#include "sensor_worker.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "sensor.h"

void sensor_task_worker(sensor_params_t *params, callback_task callback, sensor_controller_t *controller)
{
  sensor_task_t *task = malloc(sizeof(sensor_task_t));
  if (task == NULL)
  {
    free(params);
    return;
  }

  task->controller = controller;
  task->params = params;
  task->callback = callback;
  task->count = 0;
  task->total = 0;

  
  //pthread_t thread;
 // pthread_create(&thread, NULL, sensor_task_thread, task);
  //pthread_detach(thread);
}
