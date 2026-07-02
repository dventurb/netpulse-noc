#include "alert_icon.h"

GtkWidget *alert_icon_new(void)
{
  GtkWidget *image = gtk_image_new_from_file("assets/icon-alert.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(image), 72);

  return image;
}
