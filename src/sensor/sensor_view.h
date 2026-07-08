#ifndef SENSOR_VIEW_H
#define SENSOR_VIEW_H

#include <gtk/gtk.h>

#include "pagination_bar.h"
#include "stats_card.h"

#include "sensor.h"
#include "sensor_controller.h"

typedef struct sensor_view_t {

  sensor_controller_t *controller;
  
  GtkBox      *container;
  GtkGrid     *table;

  stats_card_t total_card;
  stats_card_t ok_card;
  stats_card_t warning_card;
  stats_card_t failed_card;

  pagination_bar_t  pagination_bar;

  GtkButton   *import_button;
  GtkButton   *fetch_button;

  GtkEntry    *date_filter;
  GtkDropDown *status_filter;

} sensor_view_t;


GtkBox *sensor_view_create(sensor_view_t *view, sensor_controller_t *controller);
void sensor_view_destroy(sensor_view_t *view);

void sensor_view_refresh(sensor_view_t *view);
void sensor_view_update(sensor_view_t *view);

void sensor_view_update_table(sensor_view_t *view, const sensor_t *sensors, int count);
void sensor_view_update_stats_cards(sensor_view_t *view);
void sensor_view_set_actions_enabled(sensor_view_t *view, bool is_active);

#endif
