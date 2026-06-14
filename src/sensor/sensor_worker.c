#include "sensor_worker.h"

#include <pthread.h>

static void *sensor_thread_query(void *data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  sensor_controller_execute_query(task->controller, task);

  return NULL;
}

static void *sensor_thread_file(void *data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  sensor_controller_execute_file_import(task->controller, task);

  return NULL;
}

static void *sensor_thread_api(void *data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  sensor_controller_execute_api_import(task->controller, task);

  return NULL;
}

static void *sensor_thread_search_date(void *data)
{
  sensor_task_t *task = (sensor_task_t *)data;

  sensor_controller_execute_search_date(task->controller, task);
}

void sensor_worker_start_query(sensor_controller_t *controller)
{
  sensor_task_t *task = malloc(sizeof(sensor_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->filepath = NULL;

  task->status_filter = controller->status_filter;

  strcpy(task->search_text, controller->search_text);

  task->start = pagination_start(controller->pagination);
  task->end = pagination_end(controller->pagination);

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, sensor_thread_query, task);
  pthread_detach(thread);
}

void sensor_worker_file_import(sensor_controller_t *controller, const char *filepath)
{
  sensor_task_t *task = malloc(sizeof(sensor_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->filepath = strdup(filepath);

  task->status_filter = controller->status_filter;

  strcpy(task->search_text, controller->search_text);

  task->start = pagination_start(controller->pagination);
  task->end = pagination_end(controller->pagination);

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, sensor_thread_file, task);
  pthread_detach(thread);
}

void sensor_worker_api_import(sensor_controller_t *controller)
{
  sensor_task_t *task = malloc(sizeof(sensor_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->filepath = NULL;

  task->status_filter = controller->status_filter;

  strcpy(task->search_text, controller->search_text);

  task->start = pagination_start(controller->pagination);
  task->end = pagination_end(controller->pagination);

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, sensor_thread_api, task);
  pthread_detach(thread);
}

void sensor_worker_search_date(sensor_controller_t *controller)
{
  sensor_task_t *task = malloc(sizeof(sensor_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->filepath = NULL;

  strcpy(task->search_date, controller->search_date);

  task->start = pagination_start(controller->pagination);
  task->end = pagination_end(controller->pagination);

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, sensor_thread_search_date, task);
  pthread_detach(thread);
}
