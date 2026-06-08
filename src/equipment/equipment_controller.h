#ifndef EQUIPMENT_CONTROLLER_T
#define EQUIPMENT_CONTROLLER_T

#include <glib.h>

#include "application.h"
#include "equipment.h"
#include "macros.h"
#include "pagination.h"

// forward declaration to resolve circular dependencies
typedef struct equipment_view_t equipment_view_t;

typedef struct {
  application_t *app;

  equipment_view_t *view;

  int selected_count;
  equipment_node_t *selected_node;

  int status_filter;
  int type_filter;

  int sort;
  int prev_sort;

  pagination_t pagination;
} equipment_controller_t;

typedef struct {
  int start;
  int end;

  int status_filter;
  int type_filter;

  int sort;
  int prev_sort;

  int count;
} equipment_params_t;

typedef struct {
  int total;
  int operational;
  int failed;
  int maintenance;
} equipment_stats_t;

void equipment_controller_init(equipment_controller_t *controller, equipment_view_t *view, void *data);

void equipment_controller_refresh_page(equipment_controller_t *controller);
void equipment_controller_update_table(equipment_controller_t *controller);
void equipment_controller_apply_filters(equipment_controller_t *controller, int sort, int status, int type);

void equipment_controller_add(equipment_controller_t *controller, equipment_t data);
void equipment_controller_edit(equipment_controller_t *controller, equipment_t data);

void equipment_controller_remove(equipment_controller_t *controller);
void equipment_controller_search(equipment_controller_t *controller, const char *text);
void equipment_controller_handle_toggled(equipment_controller_t *controller, int id, bool is_active);

void equipment_controller_get_stats(equipment_controller_t *controller, equipment_stats_t *stats);

gboolean on_equipment_finished(gpointer data);

#endif
