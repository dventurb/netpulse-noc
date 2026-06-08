#include "application.h"
#include <gtk/gtk.h>
#include "main_window.h"
#include "persistence.h"

int main(int argc, char **argv)
{
  application_t app;
  application_init(&app);

  load_equipments(&app, "data/equipments.bin");
  load_incidents(&app.incidents_pending, &app.incidents_history, "data/incidents.bin");

  GtkApplication *gui = gtk_application_new("com.netpulse_noc", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(gui, "activate", G_CALLBACK(main_window_create), &app);
  int status = g_application_run(G_APPLICATION(gui), argc, argv);
  g_object_unref(gui);

  application_destroy(&app);

  return status;
}
