#ifndef PING_VIEW_H
#define PING_VIEW_H

#include <gtk/gtk.h>

#include "info_card.h"
#include "unit_field.h"
#include "input_field.h"
#include "terminal_panel.h"

#include "ping_tool.h"
#include "ping_controller.h"

typedef struct ping_view_t {

  ping_controller_t *controller;

  GtkBox            *container;
  GtkStack          *stack;
  GtkListBox        *list;

  info_card_t       status_card;
  info_card_t       packets_card;
  info_card_t       average_card;
  info_card_t       execution_card;
  
  unit_field_t      count_field;
  unit_field_t      timeout_field;
  unit_field_t      packet_size_field;

  input_field_t     manual_ip_field;

  GtkCheckButton    *registered_button;
  GtkCheckButton    *manual_button;

  GtkButton         *run_button;
  GtkButton         *ping_all_button;

  terminal_panel_t  terminal;

} ping_view_t;


GtkBox *ping_view_create(ping_view_t *view, ping_controller_t *controller);

void ping_view_set_actions_enabled(ping_view_t *view, bool is_active);
void ping_view_update_list(ping_view_t *view, equipment_t *equipments, int count);
void ping_view_update_cards(ping_view_t *view);

#endif
