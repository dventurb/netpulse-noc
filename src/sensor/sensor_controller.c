#include "sensor_controller.h"

#include "sensor_worker.h"
#include "sensor_view.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static const char *const SENSOR_API_URL = "https://sensorlab.innominatum.pt/v1/sensors/export/txt";


void sensor_controller_init(sensor_controller_t *controller, sensor_view_t *view, void *data)
{
  controller->view = view;
  controller->app = (application_t *)data;

  controller->status_filter = 0;
  
  controller->search_text[0] = '\0';

  controller->pagination.page = 0;
  controller->pagination.page_size = 6;
  controller->pagination.total = sensor_get_count(&controller->app->sensors);
}

static void sensor_controller_execute_filters(sensor_controller_t *controller, sensor_task_t *task, sensor_list_t *filtered)
{
  sensor_list_t temp;
  sensor_list_init(&temp);

  sensor_list_t *list = &controller->app->sensors;

  if (strlen(task->search_text) >= 1)
    sensor_filter_by_code(list, task->search_text, &temp);
  else 
    sensor_list_clone(list, &temp);

  if (task->status_filter != 0) 
    sensor_filter_by_status(&temp, task->status_filter - 1, filtered);
  else 
    sensor_list_clone(&temp, filtered);

  sensor_list_destroy(&temp);
}

static void sensor_controller_execute_pagination(sensor_task_t *task, sensor_list_t *filtered)
{
  task->total = sensor_get_count(filtered);
  task->result = sensor_list_in_range(filtered, task->start, task->end, &task->count);
}

void sensor_controller_execute_query(sensor_controller_t *controller, sensor_task_t *task)
{
  sensor_list_t filtered;
  sensor_list_init(&filtered);

  sensor_controller_execute_filters(controller, task, &filtered);
  sensor_controller_execute_pagination(task, &filtered);

  sensor_list_destroy(&filtered);

  g_idle_add(on_sensor_finish, task);
}

void sensor_controller_reset_query(sensor_controller_t *controller)
{
  controller->status_filter = 0;
  
  controller->search_text[0] = '\0';

  controller->pagination.page = 0;
  controller->pagination.total = sensor_get_count(&controller->app->sensors);

  sensor_worker_start_query(controller); // Create new thread so the UI doesnt freeze
}

void sensor_controller_start_query(sensor_controller_t *controller)
{
  sensor_worker_start_query(controller); // Create new thread so the UI doesnt freeze
}

void sensor_controller_set_filters(sensor_controller_t *controller, int status)
{
  controller->status_filter = status;

  controller->pagination.page = 0;

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

  controller->pagination.page = 0;

  sensor_controller_start_query(controller);
}

void sensor_controller_request_file_import(sensor_controller_t *controller, const char *filepath)
{
  if (filepath == NULL) return;
  
  sensor_worker_file_import(controller, filepath); // Create new thread so the UI doesnt freeze
}

void sensor_controller_execute_file_import(sensor_controller_t *controller, sensor_task_t *task)
{
  sensor_list_t *list = &controller->app->sensors;

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
      sensor_list_insert(list, sensor);

      if (sensor.status != SENSOR_OK)
        sensor_controller_create_incident(controller, &sensor);
    }
  }

  fclose(file);

  free(task->filepath);

  sensor_controller_execute_query(controller, task);
}

void sensor_controller_request_api_import(sensor_controller_t *controller)
{
  sensor_worker_api_import(controller); // Create new thread so the UI doesnt freeze
}

void sensor_controller_execute_api_import(sensor_controller_t *controller, sensor_task_t *task)
{
  sensor_list_t *list = &controller->app->sensors;

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
      sensor_list_insert(list, sensor);
      
      if (sensor.status != SENSOR_OK)
        sensor_controller_create_incident(controller, &sensor);
    }
  }

  pclose(file);

  sensor_controller_execute_query(controller, task);
}

void sensor_controller_create_incident(sensor_controller_t *controller, const sensor_t *sensor)
{
  incident_queue_t *queue = &controller->app->incidents_pending;
  incident_list_t *list = &controller->app->incidents_history;

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

  snprintf(new.technician_name, STRING_MAX, "%s", "TODO: current_user");

  new.priority = incident_get_priority(SOURCE_SENSOR, sensor->status);

  incident_queue_enqueue(queue, new);

  save_incidents(queue, list, "data/incidents.bin");
}

void sensor_controller_get_stats(sensor_controller_t *controller, sensor_stats_t *stats)
{
  sensor_list_t *list = &controller->app->sensors;

  stats->total = sensor_get_count(list);
  stats->ok = sensor_get_number_status(list, SENSOR_OK);
  stats->failure = sensor_get_number_status(list, SENSOR_NET_FAILURE);
  stats->warning = sensor_get_number_status(list, SENSOR_WARNING);
  stats->critical = sensor_get_number_status(list, SENSOR_CRITICAL);
}

gboolean on_sensor_finish(gpointer data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  task->controller->pagination.total = task->total;

  int total_pages = pagination_total_pages(task->controller->pagination, task->total);

  if (task->controller->pagination.page >= total_pages - 1)
    task->controller->pagination.page = total_pages - 1;

  if (task->controller->pagination.page < 0)
    task->controller->pagination.page = 0;

  sensor_view_update_table(task->controller->view, task->result, task->count);
  sensor_view_update_stats_cards(task->controller->view);
  sensor_view_set_actions_enabled(task->controller->view, true);

  free(task->result);
  free(task);

  return false;
}
