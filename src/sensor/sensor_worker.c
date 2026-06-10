#include "sensor_worker.h"

#include <pthread.h>

static void *sensor_thread_import_file(void *data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  sensor_controller_execute_import_file(task->controller, task);

  return NULL;
}

static void *sensor_thread_import_api(void *data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  sensor_controller_execute_import_api(task->controller, task);

  return NULL;
}

static void *sensor_thread_filter_and_paginate(void *data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  sensor_controller_filter_and_paginate(task->controller, task);

  return NULL;
}

void sensor_worker_import_file(sensor_controller_t *controller, const char *filepath)
{
  sensor_task_t *task = malloc(sizeof(sensor_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->filepath = strdup(filepath);

  task->status_filter = controller->status_filter;

  task->start = pagination_start(controller->pagination);
  task->end = pagination_end(controller->pagination);

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, sensor_thread_import_file, task);
  pthread_detach(thread);
}

void sensor_worker_import_api(sensor_controller_t *controller)
{
  sensor_task_t *task = malloc(sizeof(sensor_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->filepath = NULL;

  task->status_filter = controller->status_filter;

  task->start = pagination_start(controller->pagination);
  task->end = pagination_end(controller->pagination);

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, sensor_thread_import_api, task);
  pthread_detach(thread);
}

void sensor_worker_filter_and_paginate(sensor_controller_t *controller)
{
  sensor_task_t *task = malloc(sizeof(sensor_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->filepath = NULL;

  task->status_filter = controller->status_filter;

  task->start = pagination_start(controller->pagination);
  task->end = pagination_end(controller->pagination);

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, sensor_thread_filter_and_paginate, task);
  pthread_detach(thread);
}
