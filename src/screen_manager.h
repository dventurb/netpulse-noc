#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <gtk/gtk.h>
#include <stdbool.h>

#include "screen.h"

typedef struct app_t app_t;

typedef struct screen_manager_t screen_manager_t;

typedef enum {
  SCREEN_DASHBOARD,
  SCREEN_EQUIPMENT,
  SCREEN_CONNECTIVITY,
  SCREEN_SENSOR,
  SCREEN_INCIDENT,
  SCREEN_CONFIGURATION,

  SCREEN_COUNT

} screen_number_t;

typedef struct {
  screen_number_t number;

  const char *text;
  const char *stack_name;

  screen_t *(*create)(app_t *app);

} screen_descriptor_t;


screen_manager_t *screen_manager_create(GtkStack *stack, app_t *app);
void screen_manager_destroy(screen_manager_t *manager);

bool screen_manager_show(screen_manager_t *manager, screen_number_t number);

#endif 
