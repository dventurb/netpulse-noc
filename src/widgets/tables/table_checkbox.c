#include "table_checkbox.h"

GtkWidget *table_checkbox_new(void)
{
  GtkWidget *check_button = gtk_check_button_new();
  gtk_widget_add_css_class(check_button, "table-checkbox");

  gtk_widget_set_size_request(check_button, 60, -1);

  return check_button;
}
