#ifndef CONNECTIVITY_VIEW_H
#define CONNECTIVITY_VIEW_H

#define CONNECTIVITY_PAGE_COUNT 4

#include <gtk/gtk.h>

#include "connectivity.h"
#include "connectivity_controller.h"

typedef struct ping_view_t {
  connectivity_controller_t *controller;

  GtkBox         *container;
  GtkStack       *stack;
  GtkListBox     *list;
  
  GtkWidget      *count;
  GtkWidget      *timeout;
  GtkWidget      *packet_size;

  GtkWidget      *manual_ip;

  GtkCheckButton *registered_button;
  GtkCheckButton *manual_button;

  GtkButton      *run_button;
  GtkButton      *ping_all_button;

  GtkTextView    *terminal;
} ping_view_t;

typedef struct connectivity_view_t{
  connectivity_controller_t *controller;

  GtkBox            *container;
  GtkStack          *stack;

  GtkToggleButton   *buttons[CONNECTIVITY_PAGE_COUNT];
  GtkToggleButton   *sidebar_buttons[CONNECTIVITY_PAGE_COUNT];

  ping_view_t ping_view;

} connectivity_view_t;

GtkBox *connectivity_view_create(connectivity_view_t *view, connectivity_controller_t *controller);

void ping_view_set_result(ping_view_t *view, ping_result_t *result);
void ping_view_update_list(ping_view_t *view, equipment_t *equipments, int count);

#endif
