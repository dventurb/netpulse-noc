#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include <gtk/gtk.h>

typedef struct {
  const char *key;
  char *dest;
} form_field_t;

typedef struct {
  const char *label;
  const char *icon;
  const char *css;
  GCallback callback;
  void *data;
} dialog_action_t;

typedef struct {
  GtkWidget *window;
  GtkWidget *form;

  const char *title;
  
  dialog_action_t dialog_action;
} dialog_config_t;

GtkWidget *create_primary_button(const char *text, const char *css);
GtkWidget *create_secondary_button(const char *text, const char *icon, const char *css);
GtkWidget *create_stat_card(void);
GtkWidget *create_text_field(GtkWidget *grid, const char *text, const char *placeholder, int row, int column);
GtkWidget *create_dropdown_field(GtkWidget *grid, const char *text, const char* const *strings, int row, int column);
GtkWidget *create_dialog_window(dialog_config_t dialog_config);
GtkWidget *create_dialog_header(GtkWidget *dialog, const char *title);
GtkWidget *create_dialog_footer(GtkWidget *dialog, dialog_action_t dialog_action);
GtkWidget *create_table_header(const char *text, int width);
GtkWidget *create_table_cell(const char *text, int width);
GtkWidget *create_table_checkbox(void);
void remove_table_rows(GtkWidget *grid);
GtkWidget *create_alert_icon(void);
GtkWidget *create_remove_primary_label(const char *text);
GtkWidget *create_remove_secundary_label(const char *text);
GtkWidget *create_summary_detail(const char *title, const char *value);

#endif
