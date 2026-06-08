#include "connectivity_worker.h"

#include <pthread.h>
#include <stdlib.h>

#include "connectivity.h"

static void *ping_task_thread(void *data)
{
  ping_task_t *task = (ping_task_t *)data;

  task->result = connectivity_run_ping(task->params->ip, task->params->count, task->params->timeout, task->params->packet_size);
  task->result->responded = connectivity_check_ping(task->result->output);

  g_idle_add(task->callback, task); //  on_ping_finished()

  return NULL;
}

void ping_task_worker(ping_params_t *params, callback_task callback, connectivity_controller_t *controller)
{
  ping_task_t *task = malloc(sizeof(ping_task_t));
  if (task == NULL)
  {
    free(params);
    return;
  }

  task->params = params;
  task->callback = callback;
  task->controller = controller;
  task->result = NULL;

  pthread_t thread;
  pthread_create(&thread, NULL, ping_task_thread, task);
  pthread_detach(thread);
}
