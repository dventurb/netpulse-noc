#ifndef UI_INVENTORY_H
#define UI_INVENTORY_H

#include "application.h"
#include "ui_main_window.h"
#include <gtk/gtk.h>

typedef struct {
  application_t *application;
  GtkWidget *dialog;
  GtkWidget *form;
} equipment_form_t;

GtkWidget *create_page_inventory(ui_t *ui);

#endif
