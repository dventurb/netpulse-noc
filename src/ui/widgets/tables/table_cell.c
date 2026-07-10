#include "table_cell.h"

GtkWidget *table_cell_new(const char *text, int width, const char *css)
{
  GtkWidget *label = gtk_label_new(text ? text : "");

  gtk_widget_add_css_class(label, css);
  
  gtk_widget_set_size_request(label, width, -1);
  gtk_label_set_single_line_mode(GTK_LABEL(label), TRUE);
  gtk_widget_set_hexpand(label, FALSE);
  gtk_widget_set_halign(label, GTK_ALIGN_FILL);

  gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
  gtk_label_set_width_chars(GTK_LABEL(label), 1);
  gtk_label_set_max_width_chars(GTK_LABEL(label), 1);

  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_set_tooltip_text(label, text);

  return label;
}
