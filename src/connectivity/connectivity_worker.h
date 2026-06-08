#ifndef TASK_H
#define TASK_H

#include "connectivity_controller.h"
#include "connectivity.h"
#include "utils.h"

typedef struct {
  ping_params_t *params;
  
  callback_task callback;

  connectivity_controller_t *controller;

  ping_result_t *result;
} ping_task_t;

void ping_single_task_worker(ping_params_t *params, callback_task callback, connectivity_controller_t *controller);
void ping_all_task_worker(ping_params_t *params, callback_task callback, connectivity_controller_t *controller);

#endif
