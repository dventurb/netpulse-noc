#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include <glib.h>

#include "application.h"
#include "sensor.h"
#include "pagination.h"

// forward declaration to resolve circular dependencies
typedef struct sensor_view_t sensor_view_t;
typedef struct sensor_task_t sensor_task_t;

typedef struct {
  application_t *app;

  sensor_view_t *view;

  int status_filter;

  pagination_t pagination;
} sensor_controller_t;

typedef struct {
  int total;
  int ok;
  int failure;
  int warning;
  int critical;
} sensor_stats_t;

void sensor_controller_init(sensor_controller_t *controller, sensor_view_t *view, void *data);

void sensor_controller_refresh_page(sensor_controller_t *controller);
void sensor_controller_update_table(sensor_controller_t *controller);
void sensor_controller_apply_filters(sensor_controller_t *controller, int status);

void sensor_controller_get_stats(sensor_controller_t *controller, sensor_stats_t *stats);

void sensor_controller_filter_and_paginate(sensor_controller_t *controller, sensor_task_t *task);

void sensor_controller_request_import_file(sensor_controller_t *controller, const char *filepath);
void sensor_controller_execute_import_file(sensor_controller_t *controller, sensor_task_t *task);

void sensor_controller_request_import_api(sensor_controller_t *controller);
void sensor_controller_execute_import_api(sensor_controller_t *controller, sensor_task_t *task);

gboolean on_sensor_finish(gpointer data);

#endif
