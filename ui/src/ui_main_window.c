#include "ui_main_window.h"

#include "application.h"

void create_main_window(GtkApplication *gui, gpointer *data)
{
  application_t *application = (application_t *) data;

  GtkWidget *window = gtk_application_window_new(gui);
  gtk_window_set_title(GTK_WINDOW(window), "NetPulse NOC");
  gtk_window_present(GTK_WINDOW(window));
}
