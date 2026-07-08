#include "tab_bar.h"

#include <stdlib.h>

// Callbacks
static void on_button_clicked(GtkToggleButton *button, gpointer data);


tab_bar_t tab_bar_new(int count, GtkOrientation orientation, tab_bar_callback callback, void *data)
{
  tab_bar_t bar;

  bar.container = GTK_BOX(gtk_box_new(orientation, 0));

  bar.buttons = NULL;

  bar.count = count;
  bar.selected = 0;

  bar.callback = callback;
  bar.data = data;

  return bar;
}

void tab_bar_destroy(tab_bar_t *bar)
{
  if (bar->buttons == NULL) return;
  free(bar->buttons);
}

void tab_bar_create_buttons(tab_bar_t *bar, const char **labels, const char **icons, const char *css)
{
  bar->buttons = malloc(sizeof(toggle_button_t) * bar->count);
  if (bar->buttons == NULL) return;

  for (int i = 0; i < bar->count; i++) 
  {
    const char *icon = icons ? icons[i] : NULL;

    bar->buttons[i] = toggle_button_new(labels[i], icon, css);

    g_signal_connect(bar->buttons[i].widget, "clicked", G_CALLBACK(on_button_clicked), bar);

    gtk_box_append(bar->container, GTK_WIDGET(bar->buttons[i].widget));
  }

  tab_bar_set_selected(bar, bar->selected);
}

void tab_bar_set_selected(tab_bar_t *bar, int index)
{
  if (bar->buttons == NULL) return;
  if (index < 0 || index >= bar->count) return;

  bar->selected = index;
  
  for (int i = 0; i < bar->count; i++)
  {
    bool is_active = i == index ? TRUE : FALSE;

    gtk_toggle_button_set_active(bar->buttons[i].widget, is_active);
  }
}

void tab_bar_set_button_icon(tab_bar_t *bar, int index, const char *icon)
{
  if (bar->buttons == NULL)  return;
  if (index < 0 || index >= bar->count) return;

  toggle_button_set_icon(&bar->buttons[index], icon);
}

void tab_bar_set_button_label(tab_bar_t *bar, int index, const char *text)
{
  if (bar->buttons == NULL)  return;
  if (index < 0 || index >= bar->count) return;

  toggle_button_set_label(&bar->buttons[index], text);
}

static int tab_bar_get_button_number(tab_bar_t *bar, GtkToggleButton *button)
{
  if (bar->buttons == NULL || button == NULL) return -1;

  for (int i = 0; i < bar->count; i++) 
    if (bar->buttons[i].widget == button) return i;

  return -1;
}

static void on_button_clicked(GtkToggleButton *button, gpointer data)
{
  tab_bar_t *bar = (tab_bar_t *)data;

  int index = tab_bar_get_button_number(bar, button);

  if (index == bar->selected)
  {
    tab_bar_set_selected(bar, index);
    return;
  }

  tab_bar_set_selected(bar, index);
  bar->callback(index, bar->data);
}
