#include "pagination_bar.h"

#include "action_button.h"

GtkWidget *pagination_button_new(pagination_t pagination, char *text, int page_number)
{
  GtkWidget *button = action_button_new(text, NULL, "default-page");
  gtk_widget_set_margin_top(button, 16);
  gtk_widget_set_margin_bottom(button, 16);
  gtk_widget_set_size_request(button, 32, 32);
  g_object_set_data(G_OBJECT(button), "page-number", GINT_TO_POINTER(page_number));

  if (page_number == pagination.current_page) 
    gtk_widget_add_css_class(button, "active-page");

  return button;
}
