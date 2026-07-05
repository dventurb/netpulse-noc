#include "ping_worker.h"

#include "ping_view.h"

#include "persistence.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


static void *ping_single_task_thread(void *data)
{
  ping_task_t *task = (ping_task_t *)data;

  task->result = connectivity_run_ping(task->params->ip, task->params->count, task->params->timeout, task->params->packet_size);
  
  task->result->responded = connectivity_check_ping(task->result->output);

  connectivity_get_ping_packets_stats(task->result);
  connectivity_get_ping_latency(task->result);

  if (task->result->responded == false && task->controller->selected_equipment != NULL)
  {
    equipment_update_status(task->controller->selected_equipment, STATUS_FAILED);
    equipment_update_last_check(task->controller->selected_equipment);
   
    ping_controller_create_equipment_incident(task->controller, task->controller->selected_equipment);

    save_equipments(&task->controller->data->equipments);
  }

  else if (task->result->responded == true && task->controller->selected_equipment != NULL)
  {
    equipment_update_status(task->controller->selected_equipment, STATUS_OPERATIONAL);
    equipment_update_last_check(task->controller->selected_equipment);

    save_equipments(&task->controller->data->equipments);
  }

  connectivity_generate_log(task->params->ip, task->result->avg_latency, task->result->responded);

  g_idle_add(task->callback, task); //  on_run_ping_finished()

  return NULL;
}

static void *ping_all_task_thread(void *data)
{
  ping_task_t *task = (ping_task_t *)data;

  equipment_node_t *node = task->controller->data->equipments.head;

  while (node != NULL)
  {
    char ip[IP_MAX];
    snprintf(ip, IP_MAX, "%s", node->data.ip_address);

    task->controller->result = connectivity_run_ping(ip, task->params->count, task->params->timeout, task->params->packet_size);

    task->controller->result->responded = connectivity_check_ping(task->controller->result->output);

    if (task->controller->result->responded == false)
    {
      equipment_update_status(&node->data, STATUS_FAILED);
      equipment_update_last_check(&node->data);
      ping_controller_create_equipment_incident(task->controller, &node->data);
    }

    else 
    {
      equipment_update_status(&node->data, STATUS_OPERATIONAL);
      equipment_update_last_check(&node->data);
    }

    connectivity_generate_log(node->data.ip_address, task->controller->result->avg_latency, task->controller->result->responded);

    g_idle_add(on_run_ping_all_equipments_finished, task->controller);

    node = node->next;
  }

  ping_view_set_actions_enabled(task->controller->view, true);

  free(task->params);
  free(task->controller->result);
  free(task);

  return NULL;
}

void ping_single_task_worker(ping_params_t *params, callback_task callback, ping_controller_t *controller)
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
  pthread_create(&thread, NULL, ping_single_task_thread, task);
  pthread_detach(thread);
}

void ping_all_task_worker(ping_params_t *params, callback_task callback, ping_controller_t *controller)
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
  pthread_create(&thread, NULL, ping_all_task_thread, task);
  pthread_detach(thread);
}
