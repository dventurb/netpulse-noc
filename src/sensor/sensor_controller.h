#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include <glib.h>

#include "app_data.h"
#include "sensor.h"
#include "pagination.h"
#include "macros.h"

// forward declaration to resolve circular dependencies
typedef struct sensor_view_t sensor_view_t;
typedef struct sensor_task_t sensor_task_t;

typedef struct {

  app_data_t *data;

  sensor_view_t *view;

  int status_filter;

  char search_text[STRING_MAX];
  char search_date[DATE_MAX];

  pagination_t pagination;

} sensor_controller_t;

typedef struct {

  char total[STRING_MAX];
  char ok[STRING_MAX];
  char failure[STRING_MAX];
  char warning_critical[STRING_MAX]; // TODO

} sensor_stats_t;


void sensor_controller_init(sensor_controller_t *controller, sensor_view_t *view, app_data_t *data);

void sensor_controller_execute_query(sensor_controller_t *controller, sensor_task_t *task);
void sensor_controller_reset_query(sensor_controller_t *controller);
void sensor_controller_start_query(sensor_controller_t *controller);

void sensor_controller_set_filters(sensor_controller_t *controller, int status);
void sensor_controller_set_search(sensor_controller_t *controller, const char *text);
void sensor_controller_set_date(sensor_controller_t *controller, const char *text);


void sensor_controller_execute_search_date(sensor_controller_t *controller, sensor_task_t *task);
void sensor_controller_start_search_date(sensor_controller_t *controller);

void sensor_controller_request_file_import(sensor_controller_t *controller, const char *filepath);
void sensor_controller_execute_file_import(sensor_controller_t *controller, sensor_task_t *task);

void sensor_controller_request_api_import(sensor_controller_t *controller);
void sensor_controller_execute_api_import(sensor_controller_t *controller, sensor_task_t *task);


void sensor_controller_create_incident(sensor_controller_t *controller, const sensor_t *sensor);

bool sensor_controller_validate_date(const char *text);

void sensor_controller_get_stats(sensor_controller_t *controller, sensor_stats_t *stats);

gboolean on_sensor_finish(gpointer data);

#endif
