#ifndef UI_INCIDENT_H
#define UI_INCIDENT_H

#include "ui_main_window.h"
#include "application.h"
#include "incident.h"
#include "ui_widgets.h"
#include <gtk/gtk.h>

typedef struct ui_incident_t {
  application_t *application;
  
  GtkWidget *window;
  GtkWidget *container;
  GtkWidget *table;

  int selected_count;
  incident_node_t *selected_node;

  pagination_t pagination;
} ui_incident_t;

typedef struct {
  application_t *application;

  GtkWidget *dialog;
  GtkWidget *form;
  GtkWidget *table;

  incident_node_t *selected_node;
} incident_form_t;

GtkWidget *create_page_incident(ui_t *ui);
void ui_incident_refresh(ui_incident_t *ui_incident);

#endif

