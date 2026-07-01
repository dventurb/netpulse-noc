#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

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

#define PAGE_COUNT 6

// forward declaration to resolve circular dependencies
typedef struct app_t app_t;

typedef struct {
  app_t *app;

  GtkWindow             *window;
  GtkStack              *stack;

  GtkToggleButton       *nav_buttons[PAGE_COUNT];

  equipment_view_t      equipment_view;
  incident_view_t       incident_view;
  connectivity_view_t   connectivity_view;
  sensor_view_t         sensor_view;
  configuration_view_t  config_view;

  equipment_controller_t      equipment_ctrl;
  incident_controller_t       incident_ctrl;
  connectivity_controller_t   connectivity_ctrl;
  sensor_controller_t         sensor_ctrl;
  configuration_controller_t  config_ctrl;
} main_window_t;


typedef void (*refresh_fn)(main_window_t *); // Function Pointer

typedef enum {
  PAGE_DASHBOARD,
  PAGE_EQUIPMENT,
  PAGE_CONNECTIVITY,
  PAGE_SENSOR,
  PAGE_INCIDENT,
  PAGE_CONFIGURATION,
} page_number_t;

typedef struct {
  page_number_t number;
  const char *text;
  const char *stack_name;
  refresh_fn refresh;
} page_config_t;


main_window_t *main_window_create(GtkApplication *gtk_app, app_t *app);

#endif
