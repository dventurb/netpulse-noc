#ifndef PING_WORKER_H
#define PING_WORKER_H

#include "connectivity.h"
#include "ping_controller.h"

#include "utils.h"

typedef struct {
  ping_params_t *params;
  
  callback_task callback;

  ping_controller_t *controller;

  ping_result_t *result;
} ping_task_t;

void ping_single_task_worker(ping_params_t *params, callback_task callback, ping_controller_t *controller);
void ping_all_task_worker(ping_params_t *params, callback_task callback, ping_controller_t *controller);

#endif
