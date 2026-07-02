#include "dialog_header.h"

GtkWidget *dialog_header_new(GtkWidget *dialog, const char *title)
{
  GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(header, 672, 72);
  gtk_widget_add_css_class(header, "dialog-header");

  GtkWidget *label = gtk_label_new(title != NULL ? title : "");
  gtk_widget_add_css_class(label, "dialog-header-title");
  gtk_widget_set_hexpand(label, TRUE);
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_margin_start(label, 24);

  GtkWidget *close_button = gtk_button_new_with_label("X");
  gtk_widget_add_css_class(close_button, "dialog-header-button");
  g_signal_connect_swapped(close_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);

  gtk_box_append(GTK_BOX(header), label);
  gtk_box_append(GTK_BOX(header), close_button);

  return header;
}

