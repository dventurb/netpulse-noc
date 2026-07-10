#include "sensor_controller.h"

#include "sensor_worker.h"
#include "sensor_view.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static const char *SENSOR_API_URL = "https://sensorlab.innominatum.pt/v1/sensors/export/txt";


void sensor_controller_init(sensor_controller_t *controller, sensor_view_t *view, app_data_t *data)
{
  controller->view = view;
  controller->data = data;

  controller->status_filter = 0;
  
  controller->search_text[0] = '\0';
  format_current_date(controller->search_date);

  int total = sensor_get_count(controller->data->sensors);
  pagination_init(&controller->pagination, total);
}

static void sensor_controller_execute_filters(sensor_controller_t *controller, sensor_task_t *task, sensor_array_t *filtered)
{
  sensor_array_t *array = &controller->data->sensors;
  
  if (array == NULL || array->sensors == NULL) return;

  sensor_array_t temp;
  sensor_array_init(&temp);

  if (strlen(task->search_text) >= 1)
    sensor_filter_by_code(array, task->search_text, &temp);

  else 
    sensor_array_clone(array, &temp);

  if (task->status_filter != 0) 
    sensor_filter_by_status(&temp, task->status_filter - 1, filtered);
  else 
    sensor_array_clone(&temp, filtered);

  sensor_array_destroy(&temp);
}

static void sensor_controller_execute_pagination(sensor_task_t *task, sensor_array_t *filtered)
{
  task->total = sensor_get_count(*filtered);
  task->result = sensor_array_in_range(filtered, task->start, task->end, &task->count);
}

void sensor_controller_execute_query(sensor_controller_t *controller, sensor_task_t *task)
{
  sensor_array_t *filtered = malloc(sizeof(sensor_array_t));
  if (filtered == NULL) return;

  filtered->sensors = NULL;
  filtered->count = 0;

  sensor_controller_execute_filters(controller, task, filtered);
  sensor_controller_execute_pagination(task, filtered);

  sensor_array_destroy(filtered);
  free(filtered);

  g_idle_add(on_sensor_finish, task);
}

void sensor_controller_reset_query(sensor_controller_t *controller)
{
  controller->status_filter = 0;
  
  controller->search_text[0] = '\0';

  int total = sensor_get_count(controller->data->sensors);
  pagination_init(&controller->pagination, total);

  sensor_worker_start_query(controller); // Create new thread so the UI doesnt freeze
}

void sensor_controller_start_query(sensor_controller_t *controller)
{
  sensor_worker_start_query(controller); // Create new thread so the UI doesnt freeze
}

void sensor_controller_set_filters(sensor_controller_t *controller, int status)
{
  controller->status_filter = status;

  controller->pagination.current_page = 0;

  sensor_controller_start_query(controller);
}

void sensor_controller_set_search(sensor_controller_t *controller, const char *text)
{
  if (text == NULL) return;

  char buffer[strlen(text) + 1];
  convert_to_uppercase(text, buffer);

  switch (detect_search_type(buffer)) 
  {
    case SEARCH_SENSOR_CODE:
      snprintf(controller->search_text, CODE_MAX, "%s", buffer);
      break;
    default:
      controller->search_text[0] = '\0';
      break;
  }

  controller->pagination.current_page = 0;


  sensor_controller_start_query(controller);
}

void sensor_controller_execute_search_date(sensor_controller_t *controller, sensor_task_t *task)
{
  sensor_array_t *array = &controller->data->sensors;

  time_t datetime = parse_date_to_timestamp(controller->search_date);
  sensor_search_by_date(array, datetime);

  sensor_controller_execute_query(controller, task);
}

void sensor_controller_start_search_date(sensor_controller_t *controller)
{
  sensor_worker_search_date(controller);
}

void sensor_controller_set_date(sensor_controller_t *controller, const char *text)
{
  if (text == NULL) return;

  controller->pagination.current_page = 0;

  snprintf(controller->search_date, DATE_MAX, "%s", text);

  sensor_controller_start_search_date(controller);
}

void sensor_controller_request_file_import(sensor_controller_t *controller, const char *filepath)
{
  if (filepath == NULL) return;
  
  sensor_worker_file_import(controller, filepath); // Create new thread so the UI doesnt freeze
}

void sensor_controller_execute_file_import(sensor_controller_t *controller, sensor_task_t *task)
{
  FILE *file = fopen(task->filepath, "r");
  if (file == NULL) return;

  char buffer[250]; // store until '\n'
  
  // ignore until found 'timestamp;sequence;site;rack...'
  while (fgets(buffer, sizeof(buffer), file))
  {
    if (buffer[0] != '#') break;  
  }

  while (fgets(buffer, sizeof(buffer), file))
  {
    sensor_t sensor = sensor_create_from_line(buffer);

    if (sensor_validate(sensor) == true)
    {
      sensor_persistence_append(&sensor);

      if (sensor.status != SENSOR_OK)
        sensor_controller_create_incident(controller, &sensor);
    }
  }

  fclose(file);
  free(task->filepath);

  sensor_persistence_sort();

  sensor_controller_execute_search_date(controller, task);
}

void sensor_controller_request_api_import(sensor_controller_t *controller)
{
  sensor_worker_api_import(controller); // Create new thread so the UI doesnt freeze
}

void sensor_controller_execute_api_import(sensor_controller_t *controller, sensor_task_t *task)
{
  char input[200];
  snprintf(input, sizeof(input), "curl -s %s", SENSOR_API_URL);

  FILE *file = popen(input, "r");
  if (file == NULL) return;

  char buffer[250]; // store until '\n'
  
  // ignore until found 'timestamp;sequence;site;rack...'
  while (fgets(buffer, sizeof(buffer), file))
  {
    if (buffer[0] != '#') break;  
  }

  while (fgets(buffer, sizeof(buffer), file))
  {
    sensor_t sensor = sensor_create_from_line(buffer);

    if (sensor_validate(sensor) == true)
    {
      sensor_persistence_append(&sensor);

      if (sensor.status != SENSOR_OK)
        sensor_controller_create_incident(controller, &sensor);
    }
  }

  pclose(file);

  sensor_controller_execute_search_date(controller, task);
}

void sensor_controller_create_incident(sensor_controller_t *controller, const sensor_t *sensor)
{
  incident_queue_t *queue = &controller->data->incidents_pending;
  incident_list_t *list = &controller->data->incidents_history;

  incident_t new;

  new.source_type = SOURCE_SENSOR;
  snprintf(new.source_id, CODE_MAX, "%s", sensor->code);

  switch (sensor->status) 
  {
    case SENSOR_WARNING:
      snprintf(new.type, STRING_MAX, "%s", "Warning");
      snprintf(new.description, DESCRIPTION_MAX, "Sensor [%s] is passing safe limits.", sensor->code);
      break;

    case SENSOR_CRITICAL:
      snprintf(new.type, STRING_MAX, "%s", "Critical");
      snprintf(new.description, DESCRIPTION_MAX, "Sensor [%s] hit critical levels.", sensor->code);
      break;

    case SENSOR_NET_FAILURE:
      snprintf(new.type, STRING_MAX, "%s", "Network Down");
      snprintf(new.description, DESCRIPTION_MAX, "Sensor [%s] did not respond.", sensor->code);
      break;

    default: return;
  }

  snprintf(new.technician_name, STRING_MAX, "%s", controller->data->current_user->name);

  new.priority = incident_get_priority(SOURCE_SENSOR, sensor->status);

  incident_queue_enqueue(queue, new);

  save_incidents(queue, list);
}

bool sensor_controller_validate_date(const char *text)
{
  if (validate_date(text)) return true;
  else return false;
}

void sensor_controller_get_stats(sensor_controller_t *controller, sensor_stats_t *stats)
{
  sensor_array_t array = controller->data->sensors;
  if (array.sensors == NULL) return;

  int total = sensor_get_count(array);
  snprintf(stats->total, STRING_MAX, "%d", total);

  int ok = sensor_get_number_status(array, SENSOR_OK);
  snprintf(stats->ok, STRING_MAX, "%d", ok);

  int failure = sensor_get_number_status(array, SENSOR_NET_FAILURE);
  snprintf(stats->failure, STRING_MAX, "%d", failure);

  // TODO
  int warning = sensor_get_number_status(array, SENSOR_WARNING);
  int critical = sensor_get_number_status(array, SENSOR_CRITICAL);
  snprintf(stats->warning_critical, STRING_MAX, "%d", warning + critical);
}

gboolean on_sensor_finish(gpointer data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  task->controller->pagination.total_items = task->total;

  int total_pages = pagination_total_pages(task->controller->pagination, task->total);

  pagination_fix_current_page(&task->controller->pagination, total_pages);

  if (task->count <= 0)
    sensor_view_update_table(task->controller->view, NULL, 0);
  else
    sensor_view_update_table(task->controller->view, task->result, task->count);

  sensor_view_update_stats_cards(task->controller->view);
  sensor_view_set_actions_enabled(task->controller->view, true);

  if (task->result != NULL) free(task->result);

  free(task);

  return false;
}
