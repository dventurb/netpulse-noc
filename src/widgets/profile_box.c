#include "profile_box.h"

GtkWidget *profile_box_new(const char *text, const char *icon_file)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_hexpand(container, TRUE);
  gtk_widget_set_margin_end(container, 24);
  gtk_widget_set_halign(container, GTK_ALIGN_END);
  gtk_widget_set_valign(container, GTK_ALIGN_CENTER);

  GtkWidget *label = gtk_label_new(text ? text : "");
  gtk_widget_add_css_class(label, "profile-name");

  GtkWidget *avatar = gtk_image_new_from_file(icon_file);
  gtk_widget_set_size_request(avatar, 32, 32);

  gtk_box_append(GTK_BOX(container), label);
  gtk_box_append(GTK_BOX(container), avatar);

  return container;
}
