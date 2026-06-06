#ifndef CONTROLLER_H
#define CONTROLLER_H 

#include <glib.h>
#include "equipment.h"
#include "incident.h"
#include "macros.h"
#include "ui_widgets.h"

// forward declaration to resolve circular dependencies
typedef struct ui_inventory_t ui_inventory_t;
typedef struct ui_incident_t ui_incident_t;
typedef struct ui_ping_configuration_t ui_ping_configuration_t;

typedef enum {
  PING_INVALID_IP_ADDRESS,
  PING_INVALID_COUNT,
  PING_INVALID_TIMEOUT,
  PING_INVALID_PACKET_SIZE,
  PING_OK
} ping_validation_t;

typedef struct {
  char ip[IP_MAX];
  int count;
  int timeout;
  int packet_size;
} ping_params_t;

typedef struct {
  int start;
  int end;

  int status_filter;
  int type_filter;

  int count;
} equipment_params_t;

typedef struct {
  int start;
  int end;

  int status_filter;
  int priority_filter;
} incident_params_t;

typedef struct {
  int selected_count;
  equipment_node_t *selected_node;

  int status_filter;
  int type_filter;

  pagination_t pagination;
} equipment_controller_t;

typedef struct {
  int selected_count;
  incident_node_t *selected_node;

  int status_filter;
  int priority_filter;

  pagination_t pagination;
} incident_controller_t;

gboolean on_ping_finished(gpointer data);
gboolean on_equipment_finished(gpointer data);
gboolean on_incident_finished(gpointer data);

void pagination_controller_get_range(pagination_t pagination, int *start, int *end);
void pagination_controller_page_number(pagination_t *pagination, int number);
void pagination_controller_previous(pagination_t *pagination);
void pagination_controller_next(pagination_t *pagination);

void connectivity_controller_ping(ui_ping_configuration_t *ui_ping, const char *ip, const char *count, const char *timeout, const char *packet_size);
void connectivity_controller_search_equipment(ui_ping_configuration_t *ui_ping, const char *text);
ping_validation_t connectivity_controller_validate_ping(const char *ip, const char *count, const char *timeout, const char *packet_size);

void equipment_controller_refresh_page(ui_inventory_t *ui_inventory);
void equipment_controller_update_table(ui_inventory_t *ui_inventory);
void equipment_controller_apply_filters(ui_inventory_t *ui_inventory, int status, int type);

void equipment_controller_add(ui_inventory_t *ui_inventory, equipment_t data);
void equipment_controller_edit(ui_inventory_t *ui_inventory, equipment_node_t *node, equipment_t data);

void equipment_controller_remove(ui_inventory_t *ui_inventory, equipment_node_t *node);
void equipment_controller_search(ui_inventory_t *ui_inventory, const char *text);
void equipment_controller_handle_toggled(ui_inventory_t *ui_inventory, int id, bool is_active);

void incident_controller_refresh_page(ui_incident_t *ui_incident);
void incident_controller_update_table(ui_incident_t *ui_incident);
void incident_controller_apply_filters(ui_incident_t *ui_incident, int status, int priority);

void incident_controller_add(ui_incident_t *ui_incident, incident_t data);
void incident_controller_process(ui_incident_t *ui_incident);
void incident_controller_resolve(ui_incident_t *ui_incident);

void incident_controller_search(ui_incident_t *ui_incident, const char *text);
void incident_controller_handle_toggled(ui_incident_t *ui_incident, int id, bool is_active);

#endif
