#include "ui_widgets.h"

GtkWidget *widget_create_icon_button(const char *text, const char *image, const char *css)
{
  GtkWidget *button, *box, *icon, *label;

  button = gtk_button_new();
  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  if (image != NULL)
  {
    icon = gtk_image_new_from_file(image);
    gtk_widget_set_size_request(icon, 18, 18);
    gtk_box_append(GTK_BOX(box), icon);
  }

  if (text != NULL)
  {
    label = gtk_label_new(text);
    gtk_box_append(GTK_BOX(box), label);
  }

  if (css != NULL)
  {
    gtk_widget_add_css_class(button, css);
  }

  gtk_button_set_child(GTK_BUTTON(button), box);

  return button;
}
