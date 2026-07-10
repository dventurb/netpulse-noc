#ifndef CONFIGURATION_VIEW_H
#define CONFIGURATION_VIEW_H

#include <gtk/gtk.h>

#include "input_field.h"
#include "table.h"
#include "stats_card.h"

#include "configuration.h"
#include "configuration_controller.h"


typedef struct {

  GtkWindow  *dialog;

  GtkWidget  *layout;

  input_field_t   equipment_field;
  input_field_t   command_field;

} configuration_form_t;

typedef struct configuration_view_t {

  configuration_controller_t *controller;
  
  GtkBox      *container;

  GtkListBox  *list;

  stats_card_t total_card;
  stats_card_t last_card;
  stats_card_t technicians_card;

  table_t table;

  GtkButton   *add_button;
  GtkButton   *revert_button;
  GtkButton   *clear_button;

  configuration_form_t form;

} configuration_view_t;


GtkBox *configuration_view_create(configuration_view_t *view, configuration_controller_t *controller);
void configuration_view_destroy(configuration_view_t *view);

void configuration_view_refresh(configuration_view_t *view);
void configuration_view_update(configuration_view_t *view);

void configuration_view_update_cards(configuration_view_t *view);
void configuration_view_set_actions_enabled(configuration_view_t *view, bool is_active);

void configuration_view_update_config_table(configuration_view_t *view, const void *result, int count);
void configuration_view_update_equipment_list(configuration_view_t *view, const void *result, int count);

#endif
