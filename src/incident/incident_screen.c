#include "incident_screen.h"

#include "app.h"
#include "screen.h"

#include "incident_view.h"
#include "incident_controller.h"

#include <stdlib.h>


typedef struct {
  screen_t parent;

  incident_view_t        view;
  incident_controller_t  controller;

} incident_screen_t;


static screen_t *incident_screen_create(app_t *app);


static const screen_descriptor_t descriptor = {
  .number     = SCREEN_INCIDENT,
  .text       = "Incidents",
  .stack_name = "INCIDENT",
  .create     = incident_screen_create,
};


static void incident_screen_destroy(screen_t *parent)
{
  incident_screen_t *screen = (incident_screen_t *)parent;

  incident_view_destroy(&screen->view);

  free(screen);
}

static void incident_screen_refresh(screen_t *parent)
{
  incident_screen_t *screen = (incident_screen_t *)parent;

  incident_view_refresh(&screen->view);
}

static screen_t *incident_screen_create(app_t *app)
{
  incident_screen_t *screen = malloc(sizeof(*screen));
  if (screen == NULL) return NULL;

  incident_controller_init(&screen->controller, &screen->view, &app->data);

  screen->parent.root = GTK_WIDGET(incident_view_create(&screen->view, &screen->controller));

  screen->parent.refresh = incident_screen_refresh;
  screen->parent.destroy = incident_screen_destroy;

  return &screen->parent;
}

const screen_descriptor_t *incident_screen_get_descriptor(void)
{
  return &descriptor;
}
