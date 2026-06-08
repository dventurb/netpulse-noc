#ifndef INCIDENT_WORKER_T
#define INCIDENT_WORKER_T

#include "incident_controller.h"
#include "utils.h"

typedef struct {
  incident_params_t *params;

  callback_task callback;

  incident_controller_t *controller;
  
  incident_t *result;
  int count;
} incident_task_t;

void incident_task_worker(incident_params_t *params, callback_task callback, incident_controller_t *controller);

#endif
