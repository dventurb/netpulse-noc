#ifndef UI_INVENTORY_H
#define UI_INVENTORY_H

#include "ui_main_window.h"
#include "application.h"
#include "equipment.h"
#include "ui_widgets.h"
#include "controller.h"
#include <gtk/gtk.h>

typedef enum {
  EQUIPMENT_FORM_ADD,
  EQUIPMENT_FORM_EDIT,
  EQUIPMENT_FORM_REMOVE
} equipment_form_mode_t;

typedef struct ui_inventory_t {
  application_t *application;
  
  GtkWidget *window;
  GtkWidget *container;
  GtkWidget *table;

  equipment_controller_t controller;
} ui_inventory_t;

typedef struct {
  application_t *application;

  GtkWidget *dialog;
  GtkWidget *form;
  GtkWidget *table;

  equipment_node_t *selected_node;

  equipment_form_mode_t mode;
} equipment_form_t;

GtkWidget *create_page_inventory(ui_t *ui);
void ui_inventory_refresh(ui_inventory_t *ui_inventory);
void ui_inventory_update_table(ui_inventory_t *ui_inventory, equipment_t *equipments, int count);
void ui_inventory_update_header(ui_inventory_t *ui_inventory);
void ui_inventory_update_stats_cards(ui_inventory_t *ui_inventory);

#endif
