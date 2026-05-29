#ifndef UI_INVENTORY_H
#define UI_INVENTORY_H

#include "ui_main_window.h"
#include "application.h"
#include "equipment.h"
#include "ui_widgets.h"
#include <gtk/gtk.h>

typedef enum {
  EQUIPMENT_FORM_ADD,
  EQUIPMENT_FORM_EDIT,
  EQUIPMENT_FORM_REMOVE
} equipment_form_mode_t;

typedef struct {
  application_t *application;
  
  GtkWidget *window;
  GtkWidget *container;
  GtkWidget *table;

  int selected_count;
  equipment_node_t *selected_node;

  pagination_t pagination;
} ui_inventory_t;

typedef struct {
  application_t *application;

  GtkWidget *dialog;
  GtkWidget *form;
  GtkWidget *table;

  equipment_form_mode_t mode;
  equipment_node_t *selected_node;
} equipment_form_t;

GtkWidget *create_page_inventory(ui_t *ui);

#endif
