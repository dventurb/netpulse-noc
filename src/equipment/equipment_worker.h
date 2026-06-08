#ifndef EQUIPMENT_WORKER_H
#define EQUIPMENT_WORKER_H

#include "equipment_controller.h"
#include "utils.h"

typedef struct {
  equipment_params_t *params;
  callback_task callback;

  equipment_controller_t *controller;

  equipment_t *result;
 
  int count;
  int total;
} equipment_task_t;

void equipment_task_worker(equipment_params_t *params, callback_task callback, equipment_controller_t *controller);

#endif
