#include "action_button.h"

GtkWidget *action_button_new(const char *text, const char *icon, const char *css)
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
