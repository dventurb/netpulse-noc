#ifndef CONNECTIVITY_VIEW_H
#define CONNECTIVITY_VIEW_H

#include <gtk/gtk.h>

#include "unit_field.h"
#include "input_field.h"
#include "tab_bar.h"

#include "connectivity.h"
#include "connectivity_controller.h"

typedef enum {
  CONNECTIVITY_TOOL_PING,
  CONNECTIVITY_TOOL_TRACEROUTE,
  CONNECTIVITY_TOOL_DNS_LOCKUP,
  CONNECTIVITY_TOOL_ARP,

  CONNECTIVITY_TOOL_COUNT
} connectivity_tools_t;

typedef struct ping_view_t {
  connectivity_controller_t *controller;

  GtkBox         *container;
  GtkStack       *stack;
  GtkListBox     *list;
  
  unit_field_t   count_field;
  unit_field_t   timeout_field;
  unit_field_t   packet_size_field;

  input_field_t   manual_ip_field;

  GtkCheckButton  *registered_button;
  GtkCheckButton  *manual_button;

  GtkButton      *run_button;
  GtkButton      *ping_all_button;

  GtkBox         *stats_cards;

  GtkTextView    *terminal;

} ping_view_t;


typedef struct connectivity_view_t{
  connectivity_controller_t *controller;

  GtkBox            *container;
  GtkStack          *stack;

  tab_bar_t   topbar_tools;
  tab_bar_t   sidebar_tools;

  ping_view_t ping_view;

} connectivity_view_t;


GtkBox *connectivity_view_create(connectivity_view_t *view, connectivity_controller_t *controller);
void connectivity_view_destroy(connectivity_view_t *view);

void ping_view_set_result(ping_view_t *view, const char *output);
char *ping_view_get_result(ping_view_t *view);
void ping_view_clear_result(ping_view_t *view);
void ping_view_update_list(ping_view_t *view, equipment_t *equipments, int count);
void ping_view_update_stats_cards(ping_view_t *view, ping_result_t result);

void ping_view_set_actions_enabled(ping_view_t *view, bool is_active);

#endif
