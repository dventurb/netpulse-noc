#include "equipment_worker.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "equipment.h"

static void *equipment_task_thread(void *data)
{
  equipment_task_t *task = (equipment_task_t *)data;

  equipment_list_t filtered;
  equipment_list_init(&filtered);

  equipment_list_t *list = &task->controller->app->equipments;

  if (task->params->sort != task->params->prev_sort)
  {
    switch (task->params->sort) 
    {
      case 1:
        equipment_list_sort_by_status(list);
        break;
      case 2:
        equipment_list_sort_by_location(list);
        break;
      case 3:
        equipment_list_sort_by_type(list);
        break;
    }

    task->controller->prev_sort = task->params->sort;
  }

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

  task->total = equipment_get_count(&filtered);

  equipment_list_destroy(&filtered);

  g_idle_add(task->callback, task); // on_equipment_finish()
  
  return NULL;
}

void equipment_task_worker(equipment_params_t *params, callback_task callback, equipment_controller_t *controller)
{
  equipment_task_t *task = malloc(sizeof(equipment_task_t));
  if (task == NULL)
  {
    free(params);
    return;
  }

  task->controller = controller;
  task->params = params;
  task->callback = callback;
  task->result = NULL;
  task->count = 0;
  task->total = 0;

  pthread_t thread;
  pthread_create(&thread, NULL, equipment_task_thread, task);
  pthread_detach(thread);
}
