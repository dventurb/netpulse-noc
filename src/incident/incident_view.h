#ifndef INCIDENT_VIEW_H
#define INCIDENT_VIEW_H

#include <gtk/gtk.h>

#include "input_field.h"
#include "dropdown_field.h"
#include "table.h"
#include "stats_card.h"

#include "incident.h"
#include "incident_controller.h"


typedef struct {

  GtkWindow  *dialog;

  GtkWidget  *layout;
  
  input_field_t  number_field;
  input_field_t  technician_field;
  input_field_t  source_id_field;
  input_field_t  type_field;
  input_field_t  description_field;

  dropdown_field_t  source_field;
  dropdown_field_t  priority_field;

} incident_form_t;

typedef struct incident_view_t {
  
  incident_controller_t *controller;
  
  GtkBox      *container;

  stats_card_t total_card;
  stats_card_t pending_card;
  stats_card_t in_progress_card;
  stats_card_t concluded_card;

  table_t table;

  GtkButton   *create_button;
  GtkButton   *resolve_button;
  GtkButton   *process_button;

  GtkDropDown *priority_filter;
  GtkDropDown *status_filter;

  incident_form_t form;

} incident_view_t;


GtkBox *incident_view_create(incident_view_t *view, incident_controller_t *controller);
void incident_view_destroy(incident_view_t *view);

void incident_view_refresh(incident_view_t *view);
void incident_view_update_table(incident_view_t *view, incident_t *incidents, int count);
void incident_view_update_header(incident_view_t *view);
void incident_view_update_stats_cards(incident_view_t *view);

#endif

