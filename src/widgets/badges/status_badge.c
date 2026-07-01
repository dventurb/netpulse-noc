#include "status_badge.h"

GtkWidget *status_badge_new(const char *text, const char *icon_file, const char *css)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "table-cell");
  gtk_widget_add_css_class(box, "status-cell");

  GtkWidget *border = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(border, "status-badge");
  
  if (css)
    gtk_widget_add_css_class(border, css);

  GtkWidget *label = gtk_label_new(text ? text : "");
  gtk_widget_add_css_class(label, "status-label");

  GtkWidget *image = gtk_image_new_from_file(icon_file);
  gtk_image_set_pixel_size(GTK_IMAGE(image), 6);

  gtk_box_append(GTK_BOX(border), image);
  gtk_box_append(GTK_BOX(border), label);
  gtk_box_append(GTK_BOX(box), border);

  return box;
}
