#ifndef CONFIGURATION_VIEW_H
#define CONFIGURATION_VIEW_H

#include <gtk/gtk.h>

#include "configuration.h"
#include "configuration_controller.h"

#include "ui_widgets.h"

typedef struct {
  GtkWindow  *dialog;

  GtkWidget  *layout;

  GtkEntry   *entry_equipment;
  GtkEntry   *entry_command;
} configuration_form_t;

typedef struct configuration_view_t {
  configuration_controller_t *controller;
  
  GtkBox      *container;
  GtkGrid     *table;
  GtkListBox  *list;

  GtkBox      *cards;
  GtkBox      *pagination_bar;

  GtkButton   *add_button;
  GtkButton   *revert_button;
  GtkButton   *clear_button;

  configuration_form_t form;
} configuration_view_t;


GtkBox *configuration_view_create(configuration_view_t *view, configuration_controller_t *controller);

void configuration_view_refresh(configuration_view_t *view);
void configuration_view_update(configuration_view_t *view);

void configuration_view_update_cards(configuration_view_t *view);
void configuration_view_set_actions_enabled(configuration_view_t *view, bool is_active);

void configuration_view_update_config_table(configuration_view_t *view, const void *result, int count);
void configuration_view_update_equipment_list(configuration_view_t *view, const void *result, int count);

#endif
