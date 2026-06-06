#ifndef UI_INCIDENT_H
#define UI_INCIDENT_H

#include "ui_main_window.h"
#include "application.h"
#include "incident.h"
#include "ui_widgets.h"
#include "controller.h"
#include <gtk/gtk.h>

typedef struct ui_incident_t {
  application_t *application;
  
  GtkWidget *window;
  GtkWidget *container;
  GtkWidget *table;

  incident_controller_t controller;
} ui_incident_t;

typedef struct {
  application_t *application;

  GtkWidget *dialog;
  GtkWidget *form;
  GtkWidget *table;
} incident_form_t;

GtkWidget *create_page_incident(ui_t *ui);
void ui_incident_refresh(ui_incident_t *ui_incident);
void ui_incident_update_header(ui_incident_t *ui_incident);
void ui_incident_update_stats_cards(ui_incident_t *ui_incident);
void ui_incident_update_table(ui_incident_t *ui_incident, incident_t *incidents, int count);

#endif

