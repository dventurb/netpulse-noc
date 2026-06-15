#include "app_windows.h"

#include "app.h"

void app_windows_init(app_windows_t *windows)
{
  windows->login = NULL;
  windows->main = NULL;
}

void app_windows_create_login(app_windows_t *windows, app_t *app)
{
  if (windows->login != NULL) return;
  windows->login = login_window_create(app->gtk_app, app);
}

void app_windows_create_main(app_windows_t *windows, app_t *app)
{
  if (windows->main != NULL) return;
  windows->main = main_window_create(app->gtk_app, app);
}

void app_windows_switch(app_windows_t *windows, app_t *app)
{
  if (app->state == APP_STATE_LOGIN)
  {
    if (windows->login == NULL)
      app_windows_create_login(windows, app);

    if (windows->main != NULL && windows->main->window)
    {
      gtk_window_destroy(windows->main->window);
      windows->main->window = NULL;
    }

    gtk_window_present(windows->login->window);
  }

  else
  {
    if (windows->main == NULL)
      app_windows_create_main(windows, app);

    if (windows->login != NULL && windows->login->window != NULL)
    {
      gtk_window_destroy(windows->login->window);
      windows->login->window = NULL;
    }

    gtk_window_present(windows->main->window);
  }
}
