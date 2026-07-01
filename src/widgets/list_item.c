#include "list_item.h"

GtkWidget *list_item_new(const char *title, const char *subtitle, const char *icon_file)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_margin_top(box, 12);
  gtk_widget_set_margin_bottom(box, 12);
  gtk_widget_set_margin_start(box, 16);

  GtkWidget *image = gtk_image_new_from_file(icon_file);
  gtk_image_set_pixel_size(GTK_IMAGE(image), 6);

  GtkWidget *text_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);

  GtkWidget *title_label = gtk_label_new(title ? title : "");
  gtk_widget_set_halign(title_label, GTK_ALIGN_START);
  gtk_widget_add_css_class(title_label, "search-item-name");

  GtkWidget *subtitle_label = gtk_label_new(subtitle ? subtitle : "");
  gtk_widget_set_halign(subtitle_label, GTK_ALIGN_START);
  gtk_widget_add_css_class(subtitle_label, "search-item-ip");

  gtk_box_append(GTK_BOX(text_box), title_label);
  gtk_box_append(GTK_BOX(text_box), subtitle_label);

  gtk_box_append(GTK_BOX(box), image);
  gtk_box_append(GTK_BOX(box), text_box);

  return box;
}
