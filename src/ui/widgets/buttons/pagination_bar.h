#ifndef PAGINATION_BAR
#define PAGINATION_BAR

#include <gtk/gtk.h>

#include "pagination.h"

GtkWidget *pagination_button_new(pagination_t pagination, char *text, int page_number);

#endif
