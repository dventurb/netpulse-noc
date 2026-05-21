#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include <gtk/gtk.h>

GtkWidget *widget_create_primary_button(const char *text, const char *css);
GtkWidget *widget_create_secondary_button(const char *text, const char *icon, const char *css);
GtkWidget *widget_create_stat_card(void);
GtkWidget *widget_add_text_field(GtkWidget *grid, const char *text, const char *placeholder, int row, int column);
GtkWidget *widget_add_dropdown_field(GtkWidget *grid, const char *text, const char* const *strings, int row, int column);

#endif
