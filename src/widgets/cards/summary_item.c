#include "summary_item.h"

GtkWidget *summary_item_new(const char *title, const char *value)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

  GtkWidget *title_label = gtk_label_new(title != NULL ? title : "");
  gtk_widget_add_css_class(title_label, "summary-title-label");

  GtkWidget *value_label = gtk_label_new(value != NULL ? value : "");
  gtk_widget_add_css_class(value_label, "summary-value-label");

  gtk_box_append(GTK_BOX(box), title_label);
  gtk_box_append(GTK_BOX(box), value_label);

  return box;
}
