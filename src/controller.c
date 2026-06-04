#include "controller.h"

#include <stdlib.h>
#include "task.h"

gboolean on_ping_finished(gpointer data)
{
  ping_task_t *task = (ping_task_t *)data;

  ui_ping_set_result(task->ui_ping, task->result);

  free(task->params); // Controller
  free(task->result); // Model 
  free(task); // Worker

  return false;
}

void connectivity_controller_ping(ui_ping_configuration_t *ui_ping, const char *ip, int count, int timeout, int packet_size)
{
  ping_params_t *params = malloc(sizeof(ping_params_t));
  if (params == NULL) return;

  strncpy(params->ip, ip, STRING_MAX - 1);
  params->ip[STRING_MAX - 1] = '\0';
  params->count = count;
  params->timeout = timeout;
  params->packet_size = packet_size;

  ping_task_worker(params, on_ping_finished, ui_ping);
}
