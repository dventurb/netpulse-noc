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

typedef struct {
  int total;
  int page;
  int page_size;
} pagination_t;

GtkWidget *create_primary_button(const char *text, const char *icon, const char *css);
GtkWidget *create_secondary_button(const char *text, const char *icon, const char *css);
GtkWidget *create_stats_card(const char *title, int value, const char *css);
GtkWidget *create_text_field(GtkWidget *grid, const char *text, const char *placeholder, int row, int column);
GtkWidget *create_unit_field(const char *text, const char *placeholder, const char *unit);
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
void remove_all_children_from_widget(GtkWidget *parent);
int pagination_total_pages(const pagination_t *pagination, int count);
int pagination_start(const pagination_t *pagination);
int pagination_end(const pagination_t *pagination);

#endif
