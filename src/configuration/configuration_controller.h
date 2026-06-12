#ifndef CONFIGURATION_CONTROLLER_H
#define CONFIGURATION_CONTROLLER_H

#include <glib.h>

#include "application.h"
#include "configuration.h"
#include "pagination.h"
#include "macros.h"

// forward declaration to resolve circular dependencies
typedef struct configuration_view_t configuration_view_t;
typedef struct configuration_task_t configuration_task_t;

typedef struct {
  application_t *app;

  configuration_view_t *view;

  equipment_node_t *selected_equipment;

  char search_text[STRING_MAX];

  pagination_t pagination;
} configuration_controller_t;

typedef struct {
  int total;
  int technicians;
} configuration_stats_t;


void configuration_controller_init(configuration_controller_t *controller, configuration_view_t *view, void *data);

void configuration_controller_execute_config_query(configuration_controller_t *controller, configuration_task_t *task);
void configuration_controller_reset_config_query(configuration_controller_t *controller);
void configuration_controller_start_config_query(configuration_controller_t *controller);

void configuration_controller_execute_equipment_query(configuration_controller_t *controller, configuration_task_t *task);
void configuration_controller_reset_equipment_query(configuration_controller_t *controller);
void configuration_controller_start_equipment_query(configuration_controller_t *controller);

void configuration_controller_add(configuration_controller_t *controller, configuration_t new);

void configuration_controller_execute_clear(configuration_controller_t *controller, configuration_task_t *task);
void configuration_controller_start_clear(configuration_controller_t *controller);

void configuration_controller_execute_revert(configuration_controller_t *controller, configuration_task_t *task);
void configuration_controller_start_revert(configuration_controller_t *controller);

void configuration_controller_set_search(configuration_controller_t *controller, const char *text);

void configuration_controller_set_selected_equipment(configuration_controller_t *controller, const char *id);
bool configuration_controller_has_selected_equipment(configuration_controller_t *controller);
equipment_t *configuration_controller_get_selected_equipment(configuration_controller_t *controller);

bool configuration_controller_validate(configuration_t new);

bool configuration_controller_is_top_stack(configuration_controller_t *controller, int number);

gboolean on_configuration_finish(gpointer data);
gboolean on_configuration_revert(gpointer data);

#endif

