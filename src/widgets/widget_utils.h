#ifndef WIDGET_UTILS_H
#define WIDGET_UTILS_H

#include <gtk/gtk.h>

void widget_remove_children(GtkWidget *parent);
void list_remove_rows(GtkWidget *list);
void table_remove_rows(GtkWidget *grid);

#endif
