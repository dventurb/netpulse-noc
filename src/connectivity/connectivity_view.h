#ifndef CONNECTIVITY_VIEW_H
#define CONNECTIVITY_VIEW_H

#include <gtk/gtk.h>

#include "tab_bar.h"

#include "connectivity.h"
#include "connectivity_controller.h"
#include "connectivity_tools.h"

typedef struct connectivity_view_t {

  connectivity_controller_t *controller;

  GtkBox            *container;
  GtkStack          *stack;

  tab_bar_t   topbar_tools;
  tab_bar_t   sidebar_tools;

  connectivity_tool_manager_t   *tool_manager;

} connectivity_view_t;


GtkBox *connectivity_view_create(connectivity_view_t *view, connectivity_controller_t *controller);
void connectivity_view_destroy(connectivity_view_t *view);

#endif
