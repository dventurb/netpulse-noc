#include "ui_widgets.h"

GtkWidget *widget_create_primary_button(const char *text, const char *css)
{
  GtkWidget *button = gtk_toggle_button_new_with_label(text != NULL ? text : "");

  if (css != NULL)
  {
    gtk_widget_add_css_class(button, css);
  }

  return button;
}
