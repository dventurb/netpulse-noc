#include "screen_manager.h"

#include "app.h"
#include "app_screens.h"

struct screen_manager_t {
  app_t *app;

  GtkStack *stack;

  screen_t *screens[SCREEN_COUNT];
  screen_number_t current_screen;
};

screen_manager_t *screen_manager_create(GtkStack *stack, app_t *app)
{
  screen_manager_t *manager = malloc(sizeof(screen_manager_t));
  if (manager == NULL) return NULL;

  manager->stack = stack;
  manager->app = app;
  manager->current_screen = 0;

  for (int i = 0; i < SCREEN_COUNT; i++)
    manager->screens[i] = NULL;

  return manager;
}

void screen_manager_destroy(screen_manager_t *manager)
{
  if (manager == NULL) return;

  for (int i = 0; i < SCREEN_COUNT; i++)
    if (manager->screens[i]) screen_destroy(manager->screens[i]);

  free(manager);
}

bool screen_manager_show(screen_manager_t *manager, screen_number_t number)
{
  if (manager == NULL) return false;

  if (number == SCREEN_DASHBOARD) return false; // TODO: Dashboard
  
  const screen_descriptor_t *descriptor = app_screens_get_descriptor(number);
  if(descriptor == NULL) return false;

  if (manager->screens[number] == NULL)
  {
    screen_t *screen = descriptor->create(manager->app);
    if (screen == NULL) return false;

    manager->screens[number] = screen;

    gtk_stack_add_named(manager->stack, screen_get_root(screen), descriptor->stack_name);
  }

  gtk_stack_set_visible_child_name(manager->stack, descriptor->stack_name);

  screen_refresh(manager->screens[number]);
  manager->current_screen = number;

  return true;
}

screen_number_t screen_manager_get_current(screen_manager_t *manager)
{
  return manager->current_screen;
}
