#ifndef TASK_H
#define TASK_H

#include "ui_connectivity.h"
#include "controller.h"
#include "connectivity.h"
#include <glib.h>

typedef gboolean (*callback_task)(gpointer data);

typedef struct {
  ping_params_t *params;
  callback_task callback;
  ui_ping_configuration_t *ui_ping;
  ping_result_t *result;
} ping_task_t;

void ping_task_worker(ping_params_t *params, callback_task callback, ui_ping_configuration_t *ui_ping);

#endif
