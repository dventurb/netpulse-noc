#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <gtk/gtk.h>
#include <ui_widgets.h>

// forward declaration to resolve circular dependencies
typedef struct app_t app_t;

typedef struct login_window_t {
  app_t *app;

  GtkWindow             *window;
  GtkStack              *stack;

  input_field_t   login_username;
  input_field_t   login_password;

  input_field_t   register_name;
  input_field_t   register_username;
  input_field_t   register_password;
} login_window_t;

login_window_t *login_window_create(GtkApplication *gtk_app, app_t *app);

#endif
