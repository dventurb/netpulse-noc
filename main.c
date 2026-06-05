#include "application.h"
#include <gtk/gtk.h>
#include "ui_main_window.h"
#include "persistence.h"

int main(int argc, char **argv)
{
  application_t application;
  application_init(&application);

  load_equipments(&application, "data/equipments.bin");
  load_incidents(&application.incidents_pending, &application.incidents_history, "data/incidents.bin");

  GtkApplication *gui = gtk_application_new("com.netpulse_noc", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(gui, "activate", G_CALLBACK(create_main_window), &application);
  int status = g_application_run(G_APPLICATION(gui), argc, argv);
  g_object_unref(gui);

  application_destroy(&application);

  return status;
}
