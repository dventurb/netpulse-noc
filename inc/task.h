#ifndef TASK_H
#define TASK_H

#include <glib.h>

#include "controller.h"

#include "ui_connectivity.h"
#include "ui_inventory.h"
#include "ui_incident.h"

#include "connectivity.h"
#include "equipment.h"
#include "incident.h"

typedef gboolean (*callback_task)(gpointer data);

typedef struct {
  ping_params_t *params;
  callback_task callback;

  ui_ping_configuration_t *ui_ping;

  ping_result_t *result;
} ping_task_t;

typedef struct {
  equipment_params_t *params;
  callback_task callback;

  ui_inventory_t *ui_inventory;

  equipment_t *result;
  int count;
} equipment_task_t;

typedef struct {
  incident_params_t *params;
  callback_task callback;

  ui_incident_t *ui_incident;
  
  incident_t *result;
  int count;
} incident_task_t;

void ping_task_worker(ping_params_t *params, callback_task callback, ui_ping_configuration_t *ui_ping);
void equipment_task_worker(equipment_params_t *params, callback_task callback, ui_inventory_t *ui_inventory);
void incident_task_worker(incident_params_t *params, callback_task callback, ui_incident_t *ui_incident);

#endif
