#ifndef SENSOR_WORKER_H
#define SENSOR_WORKER_H

#include "sensor_controller.h"
#include "utils.h"

typedef struct sensor_task_t {
  sensor_controller_t *controller;

  char *filepath;
  
  int status_filter;

  char search_text[STRING_MAX];

  int start;
  int end;

  sensor_t *result;
  int count;
  int total; 
} sensor_task_t;

void sensor_worker_start_query(sensor_controller_t *controller);

void sensor_worker_file_import(sensor_controller_t *controller, const char *filepath);
void sensor_worker_api_import(sensor_controller_t *controller);

#endif
