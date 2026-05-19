#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <gtk/gtk.h>
#include "application.h"

typedef struct {
  application_t *application;
  GtkWidget *stack;
  GtkWidget *buttons[6];
  int count;
} menu_context_t;

void create_main_window(GtkApplication *gui, gpointer data);

#endif
