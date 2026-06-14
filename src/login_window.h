#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <gtk/gtk.h>

#include "equipment_view.h"
#include "incident_view.h"
#include "connectivity_view.h"
#include "sensor_view.h"
#include "configuration_view.h"

#include "equipment_controller.h"
#include "incident_controller.h"
#include "connectivity_controller.h"
#include "sensor_controller.h"
#include "configuration_controller.h"

// forward declaration to resolve circular dependencies
typedef struct app_t app_t;

typedef struct {
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
