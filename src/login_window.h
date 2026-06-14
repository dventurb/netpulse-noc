#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <gtk/gtk.h>

// forward declaration to resolve circular dependencies
typedef struct app_t app_t;

typedef struct login_window_t {
  app_t *app;

  GtkWindow             *window;
  GtkStack              *stack;

  GtkEntry *login_username;
  GtkEntry *login_password;

  GtkEntry *register_name;
  GtkEntry *register_username;
  GtkEntry *register_password;
} login_window_t;

login_window_t *login_window_create(GtkApplication *gtk_app, app_t *app);

#endif
