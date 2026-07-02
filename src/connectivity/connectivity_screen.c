#include "connectivity_screen.h"

#include "app.h"
#include "screen.h"

#include "connectivity_view.h"
#include "connectivity_controller.h"

#include <stdlib.h>


typedef struct {
  screen_t parent;

  connectivity_view_t        view;
  connectivity_controller_t  controller;

} connectivity_screen_t;


static screen_t *connectivity_screen_create(app_t *app);


static const screen_descriptor_t descriptor = {
  .number     = SCREEN_CONNECTIVITY,
  .text       = "Connectivity",
  .stack_name = "CONNECTIVITY",
  .create     = connectivity_screen_create,
};


static void connectivity_screen_destroy(screen_t *parent)
{
  connectivity_screen_t *screen = (connectivity_screen_t *)parent;

  free(screen);
}

static void connectivity_screen_refresh(screen_t *parent)
{
  (void)parent;
  return;
}

static screen_t *connectivity_screen_create(app_t *app)
{
  connectivity_screen_t *screen = malloc(sizeof(*screen));
  if (screen == NULL) return NULL;

  connectivity_controller_init(&screen->controller, &screen->view, &app->data);

  screen->parent.root = GTK_WIDGET(connectivity_view_create(&screen->view, &screen->controller));

  screen->parent.refresh = connectivity_screen_refresh;
  screen->parent.destroy = connectivity_screen_destroy;

  return &screen->parent;
}

const screen_descriptor_t *connectivity_screen_get_descriptor(void)
{
  return &descriptor;
}
