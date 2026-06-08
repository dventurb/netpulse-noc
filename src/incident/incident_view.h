#ifndef INCIDENT_VIEW_H
#define INCIDENT_VIEW_H

#include <gtk/gtk.h>

#include "incident.h"
#include "incident_controller.h"

#include "ui_widgets.h"

typedef struct {
  GtkWindow  *dialog;
  GtkWidget  *layout;
} incident_form_t;

typedef struct incident_view_t {
  incident_controller_t *controller;
  
  GtkBox      *container;
  GtkGrid     *table;

  GtkBox      *cards;
  GtkBox      *pagination_bar;

  GtkButton   *create_button;
  GtkButton   *resolve_button;
  GtkButton   *process_button;

  GtkDropDown *priority_filter;
  GtkDropDown *status_filter;

  incident_form_t form;
} incident_view_t;


GtkBox *incident_view_create(incident_view_t *view, incident_controller_t *controller);

void incident_view_refresh(incident_view_t *view);
void incident_view_update_table(incident_view_t *view, incident_t *incidents, int count);
void incident_view_update_header(incident_view_t *view);
void incident_view_update_stats_cards(incident_view_t *view);

#endif

