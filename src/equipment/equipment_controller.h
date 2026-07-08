#ifndef EQUIPMENT_CONTROLLER_T
#define EQUIPMENT_CONTROLLER_T

#include <glib.h>

#include "app_data.h"
#include "equipment.h"
#include "macros.h"
#include "pagination.h"

// forward declaration to resolve circular dependencies
typedef struct equipment_view_t equipment_view_t;

typedef enum {

  EQUIPMENT_INVALID_NAME,
  EQUIPMENT_INVALID_VENDOR,
  EQUIPMENT_INVALID_MODEL,
  EQUIPMENT_INVALID_LOCATION,

  EQUIPMENT_INVALID_IP,
  EQUIPMENT_DUPLICATE_IP,

  EQUIPMENT_INVALID_MAC,
  EQUIPMENT_DUPLICATE_MAC,

  EQUIPMENT_VALID

} equipment_validation_t;

typedef struct {

  app_data_t *data;

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
  int total;

} equipment_params_t;

typedef struct {

  char total[STRING_MAX];
  char operational[STRING_MAX];
  char failed[STRING_MAX];
  char maintenance[STRING_MAX];

} equipment_stats_t;


void equipment_controller_init(equipment_controller_t *controller, equipment_view_t *view, app_data_t *data);

void equipment_controller_refresh_page(equipment_controller_t *controller);
void equipment_controller_update_table(equipment_controller_t *controller);
void equipment_controller_apply_filters(equipment_controller_t *controller, int sort, int status, int type);

void equipment_controller_add(equipment_controller_t *controller, equipment_t data);
void equipment_controller_edit(equipment_controller_t *controller, equipment_t data);

void equipment_controller_remove(equipment_controller_t *controller);
void equipment_controller_search(equipment_controller_t *controller, const char *text);
void equipment_controller_handle_toggled(equipment_controller_t *controller, int id, bool is_active);

bool equipment_controller_can_remove(equipment_controller_t *controller);

equipment_validation_t equipment_controller_validate_add(equipment_controller_t *controller, equipment_t equipment);

equipment_validation_t equipment_controller_validate_edit(equipment_controller_t *controller, equipment_t equipment);

void equipment_controller_get_stats(equipment_controller_t *controller, equipment_stats_t *stats);

gboolean on_equipment_finish(gpointer data);

#endif
