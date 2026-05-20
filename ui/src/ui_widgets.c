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
