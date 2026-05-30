#ifndef UI_CONNECTIVITY_H
#define UI_CONNECTIVITY_H

#include "ui_main_window.h"
#include "application.h"
#include "ui_widgets.h"
#include <gtk/gtk.h>

typedef struct {
  application_t *application;
  
  GtkWidget *window;
  GtkWidget *container;
  GtkWidget *table;

} ui_connectivity_t;

GtkWidget *create_page_connectivity(ui_t *ui);

#endif
