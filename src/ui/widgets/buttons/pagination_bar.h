#ifndef PAGINATION_BAR
#define PAGINATION_BAR

#include <gtk/gtk.h>

#include "pagination.h"

typedef void (*pagination_bar_callback)(void *data);

typedef struct {
  
  GtkBox *container;

  GtkButton *previous_button;
  GtkButton *next_button;

  GtkButton **page_buttons;
  int buttons_total;

  pagination_t *pagination;

  pagination_bar_callback callback;
  void *data;

} pagination_bar_t;


pagination_bar_t pagination_bar_new(pagination_t *pagination, pagination_bar_callback callback, void *data);

void pagination_bar_refresh(pagination_bar_t *bar);
void pagination_bar_setup_callbacks(pagination_bar_t *bar);

GtkWidget *pagination_button_new(pagination_bar_t *bar, char *text, int number);

#endif
