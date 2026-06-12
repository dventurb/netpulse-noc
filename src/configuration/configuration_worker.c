#include "configuration_worker.h"

#include <pthread.h>

static const int equipment_list_size = 15;

static void *configuration_thread_equipment_query(void *data)
{
  configuration_task_t *task = (configuration_task_t *)data;

  configuration_controller_execute_equipment_query(task->controller, task);

  return NULL;
}

static void *configuration_thread_config_query(void *data)
{
  configuration_task_t *task = (configuration_task_t *)data;

  configuration_controller_execute_config_query(task->controller, task);

  return NULL;
}

static void *configuration_thread_remove(void *data)
{
  configuration_task_t *task = (configuration_task_t *)data;

  configuration_controller_execute_remove(task->controller, task);

  return NULL;
}

void configuration_worker_start_equipment_query(configuration_controller_t *controller) 
{
  configuration_task_t *task = malloc(sizeof(configuration_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->type = TASK_EQUIPMENT;

  strcpy(task->search_text, controller->search_text);

  task->start = 0;
  task->end = equipment_list_size;

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, configuration_thread_equipment_query, task);
  pthread_detach(thread);
}

void configuration_worker_start_config_query(configuration_controller_t *controller)
{
  configuration_task_t *task = malloc(sizeof(configuration_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->type = TASK_CONFIGURATION;

  task->start = pagination_start(controller->pagination);
  task->end = pagination_end(controller->pagination);

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, configuration_thread_config_query, task);
  pthread_detach(thread);
}

void configuration_worker_remove(configuration_controller_t *controller)
{
  configuration_task_t *task = malloc(sizeof(configuration_task_t));
  if (task == NULL) return;

  task->controller = controller;

  task->type = TASK_CONFIGURATION;

  task->start = pagination_start(controller->pagination);
  task->end = pagination_end(controller->pagination);

  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, configuration_thread_remove, task);
  pthread_detach(thread);
}
