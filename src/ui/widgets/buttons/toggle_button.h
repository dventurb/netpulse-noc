#ifndef TOGGLE_BUTTON_H
#define TOGGLE_BUTTON_H

#include <gtk/gtk.h>

typedef struct {

  GtkToggleButton   *widget;
  GtkBox            *container;
  GtkImage          *icon;
  GtkLabel          *label;

} toggle_button_t;


toggle_button_t toggle_button_new(const char *text, const char *icon, const char *css);

void toggle_button_remove_child(toggle_button_t *button, GtkWidget *child);

void toggle_button_set_icon(toggle_button_t *button, const char *icon);
void toggle_button_set_label(toggle_button_t *button, const char *text);
void toggle_button_set_css_class(toggle_button_t *button, const char *css);

#endif
