#include "dropdown_field.h"

dropdown_field_t dropdown_field_new(const char *text, const char* const* strings)
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
