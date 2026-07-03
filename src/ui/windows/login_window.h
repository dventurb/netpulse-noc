#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <gtk/gtk.h>

#include "input_field.h"

// forward declaration to resolve circular dependencies
typedef struct app_t app_t;

typedef enum {
  AUTH_VIEW_LOGIN,
  AUTH_VIEW_REGISTER,
  
  AUTH_VIEW_COUNT,

} auth_view_t;

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
