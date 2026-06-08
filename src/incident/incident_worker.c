#include "incident_worker.h"

#include "stdio.h"
#include <pthread.h>
#include <stdlib.h>

#include "incident.h"

static void *incident_task_thread(void *data)
{
  incident_task_t *task = (incident_task_t *)data;

  incident_queue_t queue_filtered;
  incident_list_t list_filtered;
  incident_queue_init(&queue_filtered);
  incident_list_init(&list_filtered);

  incident_queue_t *queue = &task->controller->app->incidents_pending;
  incident_list_t *list = &task->controller->app->incidents_history;

  if (strlen(task->params->search_text) >= 1)
  {
    incident_queue_filter_by_id(queue, task->params->search_text, &queue_filtered);
    incident_list_filter_by_id(list, task->params->search_text, &list_filtered);
  }
  else 
  {
    incident_queue_clone(queue, &queue_filtered);
    incident_list_clone(list, &list_filtered);
  }

  if (task->params->status_filter != 0 || task->params->priority_filter != 0)
  {
    incident_queue_t queue_tmp;
    incident_list_t list_tmp;
    incident_queue_init(&queue_tmp);
    incident_list_init(&list_tmp);

    if (task->params->status_filter != 0 && task->params->priority_filter == 0)
    {
      incident_queue_filter_by_status(&queue_filtered, task->params->status_filter - 1, &queue_tmp);
      incident_list_filter_by_status(&list_filtered, task->params->status_filter - 1, &list_tmp);
    }

    else if (task->params->status_filter == 0 && task->params->priority_filter != 0)
    {
      incident_queue_filter_by_priority(&queue_filtered, task->params->priority_filter - 1, &queue_tmp);
      incident_list_filter_by_priority(&list_filtered, task->params->priority_filter - 1, &list_tmp);
    }

    else 
    {
      incident_queue_filter(&queue_filtered, task->params->status_filter - 1, task->params->priority_filter - 1, &queue_tmp);
      incident_list_filter(&list_filtered, task->params->status_filter - 1, task->params->priority_filter - 1, &list_tmp);
    }

    incident_queue_destroy(&queue_filtered);
    incident_list_destroy(&list_filtered);
    
    queue_filtered = queue_tmp;
    list_filtered = list_tmp;
  }

  task->result = incident_in_range(&queue_filtered, &list_filtered, task->params->start, task->params->end, &task->count);

  task->total = incident_get_count(&queue_filtered, &list_filtered);

  incident_queue_destroy(&queue_filtered);
  incident_list_destroy(&list_filtered);

  g_idle_add(task->callback, task); // on_incident_finished()
  
  return NULL;
}

void incident_task_worker(incident_params_t *params, callback_task callback, incident_controller_t *controller)
{
  incident_task_t *task = malloc(sizeof(incident_task_t));
  if (task == NULL)
  {
    free(params);
    return;
  }

  task->params = params;
  task->callback = callback;
  task->controller = controller;
  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, incident_task_thread, task);
  pthread_detach(thread);
}
