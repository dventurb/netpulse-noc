#include "input_field.h"

input_field_t input_field_new(const char *text, const char *placeholder, const char *icon_file)
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
