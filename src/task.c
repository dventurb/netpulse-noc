#include "task.h"

#include <pthread.h>
#include <stdlib.h>

#include "connectivity.h"
#include "equipment.h"
#include "incident.h"

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

  task->result = equipment_list_in_range(&filtered, task->params->start, task->params->end, &task->count);

  equipment_list_destroy(&filtered);

  g_idle_add(task->callback, task); // on_equipment_finished()
  
  return NULL;
}

static void *incident_task_thread(void *data)
{
  incident_task_t *task = (incident_task_t *)data;

  incident_queue_t queue_filtered;
  incident_list_t list_filtered;
  incident_queue_init(&queue_filtered);
  incident_list_init(&list_filtered);

  incident_queue_t *queue = &task->ui_incident->application->incidents_pending;
  incident_list_t *list = &task->ui_incident->application->incidents_history;

  if (task->params->status_filter == 0 && task->params->priority_filter == 0)
  {
    incident_queue_clone(queue, &queue_filtered);
    incident_list_clone(list, &list_filtered);
  }

  else if (task->params->status_filter != 0 && task->params->priority_filter == 0)
  {
    incident_queue_filter_by_status(queue, task->params->status_filter - 1, &queue_filtered);
    incident_list_filter_by_status(list, task->params->status_filter - 1, &list_filtered);
  }

  else if (task->params->status_filter == 0 && task->params->priority_filter != 0)
  {
    incident_queue_filter_by_priority(queue, task->params->priority_filter - 1, &queue_filtered);
    incident_list_filter_by_priority(list, task->params->priority_filter - 1, &list_filtered);
  }

  else 
  {
    incident_queue_filter_by_priority(queue, task->params->priority_filter - 1, &queue_filtered);
    incident_list_filter_by_priority(list, task->params->priority_filter - 1, &list_filtered);
  }

  task->result = incident_in_range(&queue_filtered, &list_filtered, task->params->start, task->params->end, &task->count);

  incident_queue_destroy(&queue_filtered);
  incident_list_destroy(&list_filtered);

  g_idle_add(task->callback, task); // on_incident_finished()
  
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
  task->count = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, equipment_task_thread, task);
  pthread_detach(thread);
}

void incident_task_worker(incident_params_t *params, callback_task callback, ui_incident_t *ui_incident)
{
  incident_task_t *task = malloc(sizeof(incident_task_t));
  if (task == NULL)
  {
    free(params);
    return;
  }

  task->params = params;
  task->ui_incident = ui_incident;
  task->callback = callback;
  task->result = NULL;
  task->count = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, incident_task_thread, task);
  pthread_detach(thread);
}
