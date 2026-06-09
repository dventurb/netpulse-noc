#include "sensor_controller.h"

#include "sensor_worker.h"
#include "sensor_view.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void sensor_controller_init(sensor_controller_t *controller, sensor_view_t *view, void *data)
{
  controller->view = view;
  controller->app = (application_t *)data;

  controller->pagination.page = 0;
  controller->pagination.page_size = 6;
  controller->pagination.total = controller->app->sensors.count;

  controller->status_filter = 0;
}

static void sensor_controller_dispatch(sensor_controller_t *controller)
{
  sensor_params_t *params = malloc(sizeof(sensor_params_t));
  if (params == NULL) return;

  params->start = pagination_start(controller->pagination);
  params->end = pagination_end(controller->pagination);

  params->status_filter = controller->status_filter;

  sensor_task_worker(params, on_sensor_finished, controller);
}

void sensor_controller_refresh_page(sensor_controller_t *controller)
{
  controller->pagination.page = 0;
  
  controller->pagination.total = controller->app->sensors.count;

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
  /*
  stats->total = sensor_get_count(&controller->app->equipments);
  stats->ok = sensor_get_number_status(&controller->app->equipments, SENSOR_OK);
  stats->failure = sensor_get_number_status(&controller->app->equipments, SENSOR_NET_FAILURE);
  stats->warning = sensor_get_number_status(&controller->app->equipments, SENSOR_WARNING);
  */
}

void sensor_controller_import_from_file(sensor_controller_t *controller, const char *filepath)
{
  if (filepath == NULL) return;

  sensor_insert_from_file(&controller->app->sensors, filepath);

  sensor_controller_update_table(controller);
}

gboolean on_sensor_finished(gpointer data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  task->controller->pagination.total = task->total;

  int total_pages = pagination_total_pages(task->controller->pagination, task->total);

  if (task->controller->pagination.page >= total_pages - 1)
    task->controller->pagination.page = total_pages - 1;

  sensor_view_update_table(task->controller->view, task->result, task->count);
  sensor_view_update_stats_cards(task->controller->view);

  free(task->params);
  free(task);

  return false;
}
