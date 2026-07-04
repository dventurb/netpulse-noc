#include "profile_box.h"

profile_box_t profile_box_new(const char *name, const char *icon_file)
{
  profile_box_t profile_box = {0};

  profile_box.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
  gtk_widget_set_hexpand(GTK_WIDGET(profile_box.container), TRUE);
  gtk_widget_set_margin_end(GTK_WIDGET(profile_box.container), 24);
  gtk_widget_set_halign(GTK_WIDGET(profile_box.container), GTK_ALIGN_END);
  gtk_widget_set_valign(GTK_WIDGET(profile_box.container), GTK_ALIGN_CENTER);

  profile_box.name = GTK_LABEL(gtk_label_new(name ? name : ""));
  gtk_widget_add_css_class(GTK_WIDGET(profile_box.name), "profile-name");

  profile_box.avatar = GTK_IMAGE(gtk_image_new_from_file(icon_file));
  gtk_widget_set_size_request(GTK_WIDGET(profile_box.avatar), 32, 32);

  gtk_box_append(profile_box.container, GTK_WIDGET(profile_box.name));
  gtk_box_append(profile_box.container, GTK_WIDGET(profile_box.avatar));

  return profile_box;
}
