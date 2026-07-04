#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>

#include "tab_bar.h"
#include "screen_manager.h"

// forward declaration to resolve circular dependencies
typedef struct app_t app_t;

typedef struct {
  app_t *app;

  GtkWindow             *window;
  GtkStack              *stack;

  tab_bar_t             navigation_bar;

  screen_manager_t      *screen_manager;

} main_window_t;


main_window_t *main_window_create(GtkApplication *gtk_app, app_t *app);

#endif
