#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include <gtk/gtk.h>

typedef struct {
  const char *key;
  char *dest;
} form_field_t;

GtkWidget *create_primary_button(const char *text, const char *css);
GtkWidget *create_secondary_button(const char *text, const char *icon, const char *css);
GtkWidget *create_stat_card(void);
GtkWidget *create_text_field(GtkWidget *grid, const char *text, const char *placeholder, int row, int column);
GtkWidget *create_dropdown_field(GtkWidget *grid, const char *text, const char* const *strings, int row, int column);
GtkWidget *create_dialog_window(GtkWidget *window, GtkWidget *form, const char *title, GCallback callback, gpointer data);
GtkWidget *create_dialog_header(GtkWidget *dialog, const char *title);
GtkWidget *create_dialog_footer(GtkWidget *dialog, const char *title, GCallback callback, gpointer data);

#endif
