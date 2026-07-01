#include "dialog.h"

#include "dialog_header.h"
#include "dialog_footer.h"

GtkWidget *dialog_new(dialog_config_t config)
{
  GtkWidget *dialog = gtk_window_new();
  gtk_widget_add_css_class(dialog, "dialog");
  //gtk_window_set_default_size(GTK_WINDOW(dialog), 672, 640);

  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(config.window));
  gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);
  
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(dialog), box);

  gtk_box_append(GTK_BOX(box), dialog_header_new(dialog, config.title));
  gtk_box_append(GTK_BOX(box), config.form);
  gtk_box_append(GTK_BOX(box), dialog_footer_new(dialog, config.action));

  return dialog;
}
