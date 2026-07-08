#ifndef INCIDENT_CONTROLLER_H
#define INCIDENT_CONTROLLER_H

#include <glib.h>

#include "app_data.h"
#include "incident.h"
#include "macros.h"
#include "pagination.h"
#include "utils.h"

// forward declaration to resolve circular dependencies
typedef struct incident_view_t incident_view_t;

typedef enum {

  INCIDENT_INVALID_SOURCE_ID,
  INCIDENT_INVALID_TYPE,
  INCIDENT_INVALID_DESCRIPTION,
  INCIDENT_VALID

} incident_validation_t;

typedef struct {

  app_data_t *data;

  incident_view_t *view;

  int selected_count;
  incident_node_t *selected_node;

  int status_filter;
  int priority_filter;

  char search_text[STRING_MAX];
  search_type_t search_type;

  pagination_t pagination;

} incident_controller_t;

typedef struct {

  int start;
  int end;

  int status_filter;
  int priority_filter;

  char search_text[STRING_MAX];

} incident_params_t;

typedef struct {

  char total[STRING_MAX];
  char pending[STRING_MAX];
  char in_progress[STRING_MAX];
  char concluded[STRING_MAX];

} incident_stats_t;


void incident_controller_init(incident_controller_t *controller, incident_view_t *view, app_data_t *data);

void incident_controller_refresh_page(incident_controller_t *controller);
void incident_controller_update_table(incident_controller_t *controller);
void incident_controller_apply_filters(incident_controller_t *controller, int status, int priority);

void incident_controller_add(incident_controller_t *controller, incident_t data);
void incident_controller_process(incident_controller_t *controller);
void incident_controller_resolve(incident_controller_t *controller);

void incident_controller_search(incident_controller_t *controller, const char *text);
void incident_controller_handle_toggled(incident_controller_t *controller, int id, bool is_active);
int incident_controller_get_position(incident_controller_t *controller, incident_t incident, int row);

incident_validation_t incident_controller_validate(incident_controller_t *controller, incident_t incident);

void incident_controller_get_stats(incident_controller_t *controller, incident_stats_t *stats);

gboolean on_incident_finished(gpointer data);

#endif
