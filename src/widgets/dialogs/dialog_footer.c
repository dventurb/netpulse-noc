#include "dialog_footer.h"

#include "action_button.h"

GtkWidget *dialog_footer_new(GtkWidget *dialog, dialog_action_t action)
{
  GtkWidget *footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_size_request(footer, 672, 68);
  gtk_widget_add_css_class(footer, "dialog-footer");

  GtkWidget *cancel_button = action_button_new("Cancel", NULL, "dialog-footer-cancel-button");
  gtk_widget_set_hexpand(cancel_button, TRUE);
  gtk_widget_set_halign(cancel_button, GTK_ALIGN_END);
  g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);

  GtkWidget *button = action_button_new(action.label, action.icon, action.css);
  gtk_widget_set_margin_end(button, 24);
  g_signal_connect(button, "clicked", G_CALLBACK(action.callback), action.data);

  gtk_box_append(GTK_BOX(footer), cancel_button);
  gtk_box_append(GTK_BOX(footer), button);

  return footer;
}
