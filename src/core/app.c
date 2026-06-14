#include "app.h"

// Callback
static void on_activate(GtkApplication *gtk_app, gpointer data);

void app_init(app_t *app)
{
  app_data_init(&app->data);
  app_windows_init(&app->windows);

  app->state = APP_STATE_LOGIN;

  app->gtk_app = NULL;
}

void app_destroy(app_t *app)
{
  app_data_destroy(&app->data);
  
  app->gtk_app = NULL;
}

int app_launcher(app_t *app, int argc, char **argv)
{
  app->gtk_app = gtk_application_new("com.netpulse_noc", G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect(app->gtk_app, "activate", G_CALLBACK(on_activate), app);

  int status = g_application_run(G_APPLICATION(app->gtk_app), argc, argv);

  g_object_unref(app->gtk_app);
  
  return status;
}

static void on_activate(GtkApplication *gtk_app, gpointer data)
{
  app_t *app = (app_t *)data;

  app_windows_switch(&app->windows, app);
}
