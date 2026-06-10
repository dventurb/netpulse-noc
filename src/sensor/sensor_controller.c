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

  controller->pagination.page = 0;
  controller->pagination.page_size = 6;

  controller->pagination.total = sensor_get_count(&controller->app->sensors);

  controller->status_filter = 0;
}

static void sensor_controller_dispatch(sensor_controller_t *controller)
{
  sensor_worker_filter_and_paginate(controller);
}

void sensor_controller_refresh_page(sensor_controller_t *controller)
{
  controller->pagination.page = 0;
  
  controller->pagination.total = sensor_get_count(&controller->app->sensors);

  controller->status_filter = 0;

  sensor_controller_update_table(controller);
}

void sensor_controller_update_table(sensor_controller_t *controller)
{
  sensor_controller_dispatch(controller);
}

void sensor_controller_apply_filters(sensor_controller_t *controller, int status)
{
  controller->status_filter = status;

  controller->pagination.page = 0;

  sensor_controller_update_table(controller);
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

void sensor_controller_filter_and_paginate(sensor_controller_t *controller, sensor_task_t *task)
{
  sensor_list_t *list = &controller->app->sensors;

  sensor_list_t filtered;
  sensor_list_init(&filtered);

  if (task->status_filter == 0)
    sensor_list_clone(list, &filtered);
  else 
    sensor_filter_by_status(list, task->status_filter - 1, &filtered);

  task->total = sensor_get_count(&filtered);

  task->result = sensor_list_in_range(&filtered, task->start, task->end, &task->count);

  sensor_list_destroy(&filtered);

  g_idle_add(on_sensor_finish, task);
}

void sensor_controller_request_import_file(sensor_controller_t *controller, const char *filepath)
{
  if (filepath == NULL) return;
  
  sensor_worker_import_file(controller, filepath);
}

void sensor_controller_execute_import_file(sensor_controller_t *controller, sensor_task_t *task)
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
      sensor_list_insert(list, sensor);
  }

  fclose(file);

  free(task->filepath);

  sensor_controller_filter_and_paginate(controller, task);
}

void sensor_controller_request_import_api(sensor_controller_t *controller)
{
  sensor_worker_import_api(controller); // Create new thread so the UI doesnt freeze
}

void sensor_controller_execute_import_api(sensor_controller_t *controller, sensor_task_t *task)
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
      sensor_list_insert(list, sensor);
  }

  pclose(file);

  sensor_controller_filter_and_paginate(controller, task);
}

gboolean on_sensor_finish(gpointer data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  task->controller->pagination.total = task->total;

  int total_pages = pagination_total_pages(task->controller->pagination, task->total);

  sensor_view_update_table(task->controller->view, task->result, task->count);
  sensor_view_update_stats_cards(task->controller->view);
  sensor_view_set_actions_enabled(task->controller->view, true);

  free(task->result);
  free(task);

  return false;
}
