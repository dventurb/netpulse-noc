#include "priority_badge.h"

GtkWidget *priority_badge_new(const char *text, const char *css)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_add_css_class(box, "table-cell");
  gtk_widget_add_css_class(box, "priority-cell");

  GtkWidget *border = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_widget_add_css_class(border, "priority-badge");

  if (css)
    gtk_widget_add_css_class(border, css);

  GtkWidget *label = gtk_label_new(text ? text : "");
  gtk_widget_add_css_class(label, "priority-label");

  gtk_box_append(GTK_BOX(border), label);
  gtk_box_append(GTK_BOX(box), border);

  return box;
}
