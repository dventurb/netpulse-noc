#ifndef TERMINAL_PANEL_H
#define TERMINAL_PANEL_H

#include <gtk/gtk.h>

#include "action_button.h"

typedef struct {

  GtkBox *container;

  GtkTextView   *text_view;
  GtkTextBuffer *buffer;

  GtkLabel  *title;

  GtkButton *copy_button;
  GtkButton *save_button;

} terminal_panel_t;


terminal_panel_t terminal_panel_new();

void terminal_panel_set_title(terminal_panel_t *terminal, const char *title);
void terminal_panel_set_output(terminal_panel_t *terminal, const char *output);
char *terminal_panel_get_output(terminal_panel_t *terminal);
void terminal_panel_clear_output(terminal_panel_t *terminal);

#endif
