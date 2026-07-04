#include "header_bar.h"

header_bar_t header_bar_new(const char *logo, const char *name, const char *avatar)
{
  header_bar_t bar = {0};

  bar.container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
  gtk_widget_set_size_request(GTK_WIDGET(bar.container), -1, 52);
  gtk_widget_set_hexpand(GTK_WIDGET(bar.container), TRUE);
  gtk_widget_add_css_class(GTK_WIDGET(bar.container), "header");

  bar.logo = GTK_PICTURE(gtk_picture_new_for_filename(logo));
  gtk_widget_set_halign(GTK_WIDGET(bar.logo), GTK_ALIGN_START);
  gtk_widget_set_valign(GTK_WIDGET(bar.logo), GTK_ALIGN_CENTER);
  gtk_widget_set_margin_start(GTK_WIDGET(bar.logo), 24);
  gtk_widget_set_margin_top(GTK_WIDGET(bar.logo), 4);
  gtk_widget_set_margin_bottom(GTK_WIDGET(bar.logo), 4);

  GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_hexpand(spacer, TRUE);
 
  bar.profile_box = profile_box_new(name, avatar);

  gtk_box_append(bar.container, GTK_WIDGET(bar.logo));
  gtk_box_append(bar.container, spacer);
  gtk_box_append(bar.container, GTK_WIDGET(bar.profile_box.container));

  return bar;
}
