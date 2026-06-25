#include "ui_widgets.h"

#include "macros.h"

GtkWidget *create_primary_button(const char *text, const char *icon, const char *css)
{
  GtkWidget *button = gtk_toggle_button_new();

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

GtkWidget *create_pagination_button(pagination_t pagination, char *text, int page_number)
{
  GtkWidget *button = create_secondary_button(text, NULL, "default-page");
  gtk_widget_set_margin_top(button, 16);
  gtk_widget_set_margin_bottom(button, 16);
  gtk_widget_set_size_request(button, 32, 32);
  g_object_set_data(G_OBJECT(button), "page-number", GINT_TO_POINTER(page_number));

  if (page_number == pagination.current_page) 
    gtk_widget_add_css_class(button, "active-page");

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

  char buffer[STRING_MAX];
  snprintf(buffer, STRING_MAX, "%d", value);

  GtkWidget *value_label = gtk_label_new(buffer);
  gtk_widget_set_halign(value_label, GTK_ALIGN_START);
  gtk_widget_add_css_class(value_label, "stats-card-value");

  gtk_box_append(GTK_BOX(box), title_label);
  gtk_box_append(GTK_BOX(box), value_label);

  return box;
}

input_field_t create_input_field(const char *text, const char *placeholder, const char *icon_file)
{
  input_field_t field;

  field.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));

  if (text != NULL)
  {
    GtkWidget *label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "input-field-label");
    gtk_box_append(field.container, label);
  }

  GtkWidget *input_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_widget_add_css_class(input_container, "input-field-container");
  gtk_widget_set_size_request(input_container, 336, 40);
  gtk_box_append(field.container, input_container);

  if (icon_file != NULL)
  {
    GtkWidget *icon = gtk_image_new_from_file(icon_file);
    gtk_box_append(GTK_BOX(input_container), icon);
  }

  field.entry = GTK_ENTRY(gtk_entry_new());
  gtk_widget_set_hexpand(GTK_WIDGET(field.entry), TRUE);
  gtk_widget_add_css_class(GTK_WIDGET(field.entry), "input-field-entry");

  if (placeholder != NULL)
    gtk_entry_set_placeholder_text(field.entry, placeholder);

  gtk_box_append(GTK_BOX(input_container), GTK_WIDGET(field.entry));

  return field;
}

unit_field_t create_unit_field(const char *text, const char *placeholder, const char *unit)
{
  unit_field_t field;

  field.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));

  if (text != NULL)
  {
    GtkWidget *label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "unit-field-label");
    gtk_box_append(field.container, label);
  }

  GtkWidget *input_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_widget_add_css_class(input_container, "unit-field-container");
  gtk_widget_set_size_request(input_container, 336, 40);
  gtk_box_append(field.container, input_container);

  field.entry = GTK_ENTRY(gtk_entry_new());
  gtk_widget_set_hexpand(GTK_WIDGET(field.entry), TRUE);
  gtk_widget_add_css_class(GTK_WIDGET(field.entry), "unit-field-entry");
  gtk_box_append(GTK_BOX(input_container), GTK_WIDGET(field.entry));

  if (placeholder != NULL) 
    gtk_entry_set_placeholder_text(field.entry, placeholder);

  if (unit != NULL)
  {
    field.unit_label = GTK_LABEL(gtk_label_new(unit));
    gtk_widget_add_css_class(GTK_WIDGET(field.unit_label), "unit-field-unit");
    gtk_box_append(GTK_BOX(input_container), GTK_WIDGET(field.unit_label));
  }

  return field;
}

dropdown_field_t create_dropdown_field(const char *text, const char* const* strings)
{
  dropdown_field_t field;

  field.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));

  if (text != NULL)
  {
    GtkWidget *label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_add_css_class(label, "input-field-label");
    gtk_box_append(field.container, label);
  }

  field.dropdown = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(strings));
  gtk_widget_set_hexpand(GTK_WIDGET(field.dropdown), TRUE);
  gtk_widget_add_css_class(GTK_WIDGET(field.dropdown), "dropdown-field");
  gtk_widget_set_size_request(GTK_WIDGET(field.dropdown), 336, 40);
  gtk_box_append(field.container, GTK_WIDGET(field.dropdown));

  return field;
}

GtkWidget *create_dialog_window(dialog_config_t dialog_config)
{
  GtkWidget *dialog = gtk_window_new();
  gtk_widget_add_css_class(dialog, "dialog");
  //gtk_window_set_default_size(GTK_WINDOW(dialog), 672, 640);

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

  gtk_label_set_xalign(GTK_LABEL(label), 0.0);

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
  if (grid == NULL) return;

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

void remove_list_rows(GtkWidget *list)
{
  GtkWidget *child = gtk_widget_get_first_child(list);
  while (child != NULL)
  {
    gtk_list_box_remove(GTK_LIST_BOX(list), child);
    child = gtk_widget_get_first_child(list);
  }
}
