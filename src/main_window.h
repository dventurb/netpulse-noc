#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>

#include "equipment_view.h"
#include "incident_view.h"
#include "connectivity_view.h"

#include "equipment_controller.h"
#include "incident_controller.h"
#include "connectivity_controller.h"

#define PAGE_COUNT 6

typedef struct {
  GtkWindow             *window;
  GtkStack              *stack;

  GtkToggleButton       *nav_buttons[PAGE_COUNT];

  equipment_view_t      equipment_view;
  incident_view_t       incident_view;
  connectivity_view_t   connectivity_view;

  equipment_controller_t      equipment_ctrl;
  incident_controller_t       incident_ctrl;
  connectivity_controller_t   connectivity_ctrl;
} main_window_t;

void main_window_create(GtkApplication *gui, gpointer data);

#endif
