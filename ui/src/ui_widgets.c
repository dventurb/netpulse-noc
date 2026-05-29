#include "ui_widgets.h"

GtkWidget *create_primary_button(const char *text, const char *css)
{
  GtkWidget *button = gtk_toggle_button_new_with_label(text != NULL ? text : "");

  if (css != NULL)
  {
    gtk_widget_add_css_class(button, css);
  }

  return button;
}

GtkWidget *create_secondary_button(const char *text, const char *icon, const char *css)
{
  GtkWidget *button = gtk_button_new();

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
  gtk_button_set_child(GTK_BUTTON(button), box);

  if (icon != NULL)
  {
    GtkWidget *image = gtk_image_new_from_file(icon);
    gtk_box_append(GTK_BOX(box), image);
  }

  if (text != NULL)
  {
    GtkWidget *label = gtk_label_new(text);
    gtk_box_append(GTK_BOX(box), label);
  }

  if (css != NULL)
  {
    gtk_widget_add_css_class(button, css);
  }

  return button;
}

GtkWidget *create_stats_card(const char *title, int value, const char *css)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_add_css_class(box, "stats-card");
  gtk_widget_add_css_class(box, css != NULL ? css : "");

  GtkWidget *title_label = gtk_label_new(title != NULL ? title : "");
  gtk_widget_set_halign(title_label, GTK_ALIGN_START);
  gtk_widget_add_css_class(title_label, "stats-card-title");

  char buffer[11];
  snprintf(buffer, sizeof(buffer), "%d", value);

  GtkWidget *value_label = gtk_label_new(buffer);
  gtk_widget_set_halign(value_label, GTK_ALIGN_START);
  gtk_widget_add_css_class(value_label, "stats-card-value");

  gtk_box_append(GTK_BOX(box), title_label);
  gtk_box_append(GTK_BOX(box), value_label);

  return box;
}

GtkWidget *create_text_field(GtkWidget *grid, const char *text, const char *placeholder, int row, int column)
{
  GtkWidget *box, *label, *entry;

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

  if (text != NULL)
  {
    label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "form-label");
    gtk_box_append(GTK_BOX(box), label);
  }

  entry = gtk_entry_new();
  gtk_widget_set_hexpand(entry, TRUE);
  gtk_widget_add_css_class(entry, "form-entry");
  gtk_box_append(GTK_BOX(box), entry);

  if (placeholder != NULL) gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);

  gtk_grid_attach(GTK_GRID(grid), box, column, row, 1, 1);

  return entry;
}

GtkWidget *create_dropdown_field(GtkWidget *grid, const char *text, const char* const* strings, int row, int column)
{
  GtkWidget *box, *label, *dropdown;

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

  if (text != NULL)
  {
    label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "form-label");
    gtk_box_append(GTK_BOX(box), label);
  }
  
  if (strings != NULL) {
    dropdown = gtk_drop_down_new_from_strings(strings);
    gtk_widget_set_hexpand(dropdown, TRUE);
    gtk_widget_add_css_class(dropdown, "form-dropdown");
    gtk_box_append(GTK_BOX(box), dropdown);
  }

  gtk_grid_attach(GTK_GRID(grid), box, column, row, 1, 1);

  return dropdown;
}

GtkWidget *create_dialog_window(dialog_config_t dialog_config)
{
  GtkWidget *dialog = gtk_window_new();
  gtk_widget_add_css_class(dialog, "dialog");
  gtk_window_set_default_size(GTK_WINDOW(dialog), 672, 640);

  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(dialog_config.window));
  gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);
  
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(dialog), box);

  gtk_box_append(GTK_BOX(box), create_dialog_header(dialog, dialog_config.title));
  gtk_box_append(GTK_BOX(box), dialog_config.form);
  gtk_box_append(GTK_BOX(box), create_dialog_footer(dialog, dialog_config.dialog_action));

  return dialog;
}

GtkWidget *create_dialog_header(GtkWidget *dialog, const char *title)
{
  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(header, 672, 72);
  gtk_widget_add_css_class(header, "dialog-header");

  GtkWidget *label = gtk_label_new(title != NULL ? title : "");
  gtk_widget_add_css_class(label, "dialog-header-title");
  gtk_widget_set_hexpand(label, TRUE);
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_margin_start(label, 24);

  GtkWidget *close_button = gtk_button_new_with_label("X");
  gtk_widget_add_css_class(close_button, "dialog-header-button");
  g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);

  gtk_box_append(GTK_BOX(header), label);
  gtk_box_append(GTK_BOX(header), close_button);

  return header;
}

GtkWidget *create_dialog_footer(GtkWidget *dialog, dialog_action_t dialog_action)
{
  GtkWidget *footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_size_request(footer, 672, 68);
  gtk_widget_add_css_class(footer, "dialog-footer");

  GtkWidget *cancel_button = create_secondary_button("Cancel", NULL, "dialog-footer-cancel-button");
  gtk_widget_set_hexpand(cancel_button, TRUE);
  gtk_widget_set_halign(cancel_button, GTK_ALIGN_END);
  g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);

  GtkWidget *button = create_secondary_button(dialog_action.label, dialog_action.icon, dialog_action.css);
  gtk_widget_set_margin_end(button, 24);
  g_signal_connect(button, "clicked", G_CALLBACK(dialog_action.callback), dialog_action.data);

  gtk_box_append(GTK_BOX(footer), cancel_button);
  gtk_box_append(GTK_BOX(footer), button);

  return footer;
}

GtkWidget *create_table_header(const char *text, int width)
{
  GtkWidget *label = gtk_label_new(text != NULL ? text : "");

  gtk_widget_add_css_class(label, "table-header");

  gtk_widget_set_size_request(label, width, -1);
  gtk_widget_set_hexpand(label, FALSE);
  gtk_widget_set_halign(label, GTK_ALIGN_FILL);

  gtk_label_set_xalign(GTK_LABEL(label), 0.5);

  return label;
}

GtkWidget *create_table_cell(const char *text, int width)
{
  GtkWidget *label = gtk_label_new(text != NULL ? text : "");

  gtk_widget_add_css_class(label, "table-cell");
  
  gtk_widget_set_size_request(label, width, -1);
  gtk_label_set_single_line_mode(GTK_LABEL(label), TRUE);
  gtk_widget_set_hexpand(label, FALSE);
  gtk_widget_set_halign(label, GTK_ALIGN_FILL);

  gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
  gtk_label_set_width_chars(GTK_LABEL(label), 1);
  gtk_label_set_max_width_chars(GTK_LABEL(label), 1);

  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_set_tooltip_text(label, text);

  return label;
}

GtkWidget *create_table_checkbox(void)
{
  GtkWidget *check_button = gtk_check_button_new();
  gtk_widget_add_css_class(check_button, "table-checkbox");

  gtk_widget_set_size_request(check_button, 60, -1);

  return check_button;
}

void remove_table_rows(GtkWidget *grid)
{
  GtkWidget *item = gtk_widget_get_first_child(grid);
  
  while (item != NULL)
  {
    GtkWidget *next = gtk_widget_get_next_sibling(item);
    
    int row;
    gtk_grid_query_child(GTK_GRID(grid), item, NULL, &row, NULL, NULL);
    
    if (row > 0)  gtk_grid_remove(GTK_GRID(grid), item); // Skip the headers

    item = next;
  }
}

GtkWidget *create_alert_icon(void)
{
  GtkWidget *image = gtk_image_new_from_file("assets/icon-alert.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(image), 72);

  return image;
}

GtkWidget *create_remove_primary_label(const char *text)
{
  if (text == NULL) return NULL;

  GtkWidget *label = gtk_label_new(text);
  gtk_widget_add_css_class(label, "remove-primary-label");

  return label;
}

GtkWidget *create_remove_secundary_label(const char *text)
{
  if (text == NULL) return NULL;

  GtkWidget *label = gtk_label_new(text);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
  gtk_widget_add_css_class(label, "remove-secundary-label");

  return label;
}

GtkWidget *create_summary_detail(const char *title, const char *value)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

  GtkWidget *title_label = gtk_label_new(title != NULL ? title : "");
  gtk_widget_add_css_class(title_label, "summary-title-label");

  GtkWidget *value_label = gtk_label_new(value != NULL ? value : "");
  gtk_widget_add_css_class(value_label, "summary-value-label");

  gtk_box_append(GTK_BOX(box), title_label);
  gtk_box_append(GTK_BOX(box), value_label);

  return box;
}

void remove_all_children_from_widget(GtkWidget *parent) 
{
  if (parent == NULL) return;

  GtkWidget *child = gtk_widget_get_first_child(parent);

  while (child != NULL)
  {
    GtkWidget *next = gtk_widget_get_next_sibling(child);
    
    gtk_widget_unparent(child);

    child = next;
  }
}

int pagination_total_pages(const pagination_t *pagination)
{
  return (pagination->total + pagination->page_size - 1) / pagination->page_size;
}

int pagination_start(const pagination_t *pagination)
{
  return pagination->page * pagination->page_size;
}

int pagination_end(const pagination_t *pagination)
{
  return pagination->page * pagination->page_size + pagination->page_size;
}
