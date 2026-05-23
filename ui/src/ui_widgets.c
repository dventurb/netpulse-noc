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

GtkWidget *create_dialog_window(GtkWidget *window, GtkWidget *form, const char *title, GCallback callback, gpointer data)
{
  GtkWidget *dialog = gtk_window_new();
  gtk_widget_add_css_class(dialog, "dialog");
  gtk_window_set_default_size(GTK_WINDOW(dialog), 672, 640);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
  gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);
  
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(dialog), box);

  gtk_box_append(GTK_BOX(box), create_dialog_header(dialog, title != NULL ? title : ""));
  gtk_box_append(GTK_BOX(box), form);
  gtk_box_append(GTK_BOX(box), create_dialog_footer(dialog, title != NULL ? title : "", callback, data));

  return dialog;
}

GtkWidget *create_dialog_header(GtkWidget *dialog, const char *title)
{
  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(header, "dialog-header");

  GtkWidget *label = gtk_label_new(title);
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

GtkWidget *create_dialog_footer(GtkWidget *dialog, const char *title, GCallback callback, gpointer data)
{
  GtkWidget *footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_add_css_class(footer, "dialog-footer");

  GtkWidget *cancel_button = create_secondary_button("Cancel", NULL, "dialog-footer-cancel-button");
  gtk_widget_set_hexpand(cancel_button, TRUE);
  gtk_widget_set_halign(cancel_button, GTK_ALIGN_END);
  g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);

  GtkWidget *add_button = create_secondary_button(title, "assets/icon-add-device.svg", "dialog-footer-add-button");
  gtk_widget_set_margin_end(add_button, 24);
  g_signal_connect(add_button, "clicked", G_CALLBACK(callback), data);

  gtk_box_append(GTK_BOX(footer), cancel_button);
  gtk_box_append(GTK_BOX(footer), add_button);

  return footer;
}

GtkWidget *create_table_label(const char *text, const char *css, int width)
{
  GtkWidget *label = gtk_label_new(text != NULL ? text : "");
  
  gtk_widget_add_css_class(label, css);

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

GtkWidget *create_status_cell(equipment_status_t status)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "table-cell");
  gtk_widget_add_css_class(box, "status-cell");

  GtkWidget *label = gtk_label_new(equipment_status_to_string(status));
  gtk_widget_add_css_class(label, "status-label");

  switch (status) 
  {
    case STATUS_OPERATIONAL: 
      gtk_widget_add_css_class(label, "status-operational");
      break;
    case STATUS_FAILED: 
      gtk_widget_add_css_class(label, "status-failed");
      break;
    case STATUS_MAINTENANCE: 
      gtk_widget_add_css_class(label, "status-maintenance");
      break;
    case STATUS_DISABLED:
      gtk_widget_add_css_class(label, "status-disabled");
  }

  gtk_box_append(GTK_BOX(box), label);

  return box;
}

void remove_all_items_table(GtkWidget *grid)
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
