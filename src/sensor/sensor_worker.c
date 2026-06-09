#include "sensor_worker.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


static void *sensor_thread_import_api(void *data)
{
  sensor_controller_t *controller = (sensor_controller_t *)data;

  sensor_controller_execute_import_api(controller);

  g_idle_add(on_sensor_import_api_finish, controller);

  return NULL;
}

void sensor_worker_import_api(sensor_controller_t *controller)
{
  pthread_t thread;
  pthread_create(&thread, NULL, sensor_thread_import_api, controller);
  pthread_detach(thread);
}
