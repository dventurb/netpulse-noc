#include "stats_card.h"

#include "macros.h"

GtkWidget *stats_card_new(const char *title, int value, const char *css)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_add_css_class(box, "stats-card");
  gtk_widget_add_css_class(box, css != NULL ? css : "");

  GtkWidget *title_label = gtk_label_new(title != NULL ? title : "");
  gtk_widget_set_halign(title_label, GTK_ALIGN_START);
  gtk_widget_add_css_class(title_label, "stats-card-title");

  char buffer[STRING_MAX];
  snprintf(buffer, STRING_MAX, "%d", value);

  GtkWidget *value_label = gtk_label_new(buffer);
  gtk_widget_set_halign(value_label, GTK_ALIGN_START);
  gtk_widget_add_css_class(value_label, "stats-card-value");

  gtk_box_append(GTK_BOX(box), title_label);
  gtk_box_append(GTK_BOX(box), value_label);

  return box;
}
