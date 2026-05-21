#include "ui_widgets.h"

GtkWidget *widget_create_primary_button(const char *text, const char *css)
{
  GtkWidget *button = gtk_toggle_button_new_with_label(text != NULL ? text : "");

  if (css != NULL)
  {
    gtk_widget_add_css_class(button, css);
  }

  return button;
}

GtkWidget *widget_create_secondary_button(const char *text, const char *icon, const char *css)
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

GtkWidget *widget_add_text_field(GtkWidget *grid, const char *text, const char *placeholder, int row, int column)
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

GtkWidget *widget_add_dropdown_field(GtkWidget *grid, const char *text, const char* const* strings, int row, int column)
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
