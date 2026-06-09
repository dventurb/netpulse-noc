#ifndef SENSOR_WORKER_H
#define SENSOR_WORKER_H

#include "sensor_controller.h"
#include "utils.h"

typedef struct {
  sensor_params_t *params;
  callback_task callback;

  sensor_controller_t *controller;

  int count;
  int total;

  sensor_t *result;
} sensor_task_t;

void sensor_task_worker(sensor_params_t *params, callback_task callback, sensor_controller_t *controller);

#endif
