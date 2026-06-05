#include "task.h"

#include "connectivity.h"
#include "equipment.h"
#include "ui_inventory.h"
#include <pthread.h>
#include <stdlib.h>

static void *ping_task_thread(void *data)
{
  ping_task_t *task = (ping_task_t *)data;

  task->result = connectivity_ping(task->params->ip, task->params->count, task->params->timeout, task->params->packet_size);

  g_idle_add(task->callback, task); //  on_ping_finished()

  return NULL;
}

static void *equipment_task_thread(void *data)
{
  equipment_task_t *task = (equipment_task_t *)data;

  equipment_list_t filtered;
  equipment_list_init(&filtered);

  equipment_list_t *list = &task->ui_inventory->application->equipments;

  if (task->params->type_filter == 0 && task->params->status_filter == 0)
  {
    equipment_list_clone(list, &filtered);
  }

  else if (task->params->status_filter == 0 && task->params->type_filter != 0)
  {
    equipment_filter_by_type(list, task->params->type_filter - 1, &filtered);
  }

  else if (task->params->type_filter == 0 && task->params->status_filter != 0)
  {
    equipment_filter_by_status(list, task->params->status_filter - 1, &filtered);
  }

  else 
  {
    equipment_filter(list, task->params->status_filter - 1, task->params->type_filter - 1, &filtered);
  }

  int count = 0;
  task->result = equipment_list_in_range(&filtered, task->params->start, task->params->end, &count);
  task->count = count;

  equipment_list_destroy(&filtered);

  g_idle_add(task->callback, task); // on_equipment_finished()
  
  return NULL;
}

void ping_task_worker(ping_params_t *params, callback_task callback, ui_ping_configuration_t *ui_ping)
{
  ping_task_t *task = malloc(sizeof(ping_task_t));
  if (task == NULL)
  {
    free(params);
    return;
  }

  task->params = params;
  task->callback = callback;
  task->ui_ping = ui_ping;
  task->result = NULL;

  pthread_t thread;
  pthread_create(&thread, NULL, ping_task_thread, task);
  pthread_detach(thread);
}

void equipment_task_worker(equipment_params_t *params, callback_task callback, ui_inventory_t *ui_inventory)
{
  equipment_task_t *task = malloc(sizeof(equipment_task_t));
  if (task == NULL)
  {
    free(params);
    return;
  }

  task->params = params;
  task->ui_inventory = ui_inventory;
  task->callback = callback;
  task->result = NULL;

  pthread_t thread;
  pthread_create(&thread, NULL, equipment_task_thread, task);
  pthread_detach(thread);
}
