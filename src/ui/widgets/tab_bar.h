#ifndef TAB_BAR_H
#define TAB_BAR_H

#include <gtk/gtk.h>

#include "toggle_button.h"

typedef void (*tab_bar_callback)(int index, void *data);

typedef struct {
  GtkBox *container;

  toggle_button_t *buttons;

  int count;
  int selected;

  tab_bar_callback callback;
  void *data;

} tab_bar_t;


tab_bar_t tab_bar_new(int count, GtkOrientation orientation, tab_bar_callback callback, void *data);
void tab_bar_destroy(tab_bar_t *bar);

void tab_bar_create_buttons(tab_bar_t *bar, const char **labels, const char **icons, const char *css);

void tab_bar_set_selected(tab_bar_t *bar, int index);
void tab_bar_set_button_icon(tab_bar_t *bar, int index, const char *icon);
void tab_bar_set_button_label(tab_bar_t *bar, int index, const char *text);

#endif
