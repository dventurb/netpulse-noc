#include "configuration_screen.h"

#include "app.h"
#include "screen.h"

#include "configuration_view.h"
#include "configuration_controller.h"

#include <stdlib.h>


typedef struct {
  screen_t parent;

  configuration_view_t        view;
  configuration_controller_t  controller;

} configuration_screen_t;


static screen_t *configuration_screen_create(app_t *app);


static const screen_descriptor_t descriptor = {
  .number     = SCREEN_CONFIGURATION,
  .text       = "Configurations",
  .stack_name = "CONFIGURATION",
  .create     = configuration_screen_create,
};


static void configuration_screen_destroy(screen_t *parent)
{
  configuration_screen_t *screen = (configuration_screen_t *)parent;

  configuration_view_destroy(&screen->view);

  free(screen);
}

static void configuration_screen_refresh(screen_t *parent)
{
  configuration_screen_t *screen = (configuration_screen_t *)parent;

  configuration_view_refresh(&screen->view);
}

static screen_t *configuration_screen_create(app_t *app)
{
  configuration_screen_t *screen = malloc(sizeof(*screen));
  if (screen == NULL) return NULL;

  configuration_controller_init(&screen->controller, &screen->view, &app->data);

  screen->parent.root = GTK_WIDGET(configuration_view_create(&screen->view, &screen->controller));

  screen->parent.refresh = configuration_screen_refresh;
  screen->parent.destroy = configuration_screen_destroy;

  return &screen->parent;
}

const screen_descriptor_t *configuration_screen_get_descriptor(void)
{
  return &descriptor;
}
