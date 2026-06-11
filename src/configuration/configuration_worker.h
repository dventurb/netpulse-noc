#ifndef CONFIGURATION_WORKER_H
#define CONFIGURATION_WORKER_H

#include "configuration_controller.h"

typedef enum {
  TASK_EQUIPMENT,
  TASK_CONFIGURATION
} task_type_t;

typedef struct configuration_task_t {
  configuration_controller_t *controller;

  task_type_t type;

  char search_text[STRING_MAX];

  int start;
  int end;

  void *result;
  int count;
  int total; 
} configuration_task_t;

void configuration_worker_start_equipment_query(configuration_controller_t *controller);
void configuration_worker_start_config_query(configuration_controller_t *controller);

#endif
