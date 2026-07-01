#include "table_header.h"

GtkWidget *table_header_new(const char *text, int width)
{
  GtkWidget *label = gtk_label_new(text != NULL ? text : "");

  gtk_widget_add_css_class(label, "table-header");

  gtk_widget_set_size_request(label, width, -1);
  gtk_widget_set_hexpand(label, FALSE);
  gtk_widget_set_halign(label, GTK_ALIGN_FILL);

  gtk_label_set_xalign(GTK_LABEL(label), 0.0);

  return label;
}
