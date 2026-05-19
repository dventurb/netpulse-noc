#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include <gtk/gtk.h>

GtkWidget *widget_create_icon_button(const char *text, const char *image, const char *css);
GtkWidget *widget_create_stat_card(void);

#endif
