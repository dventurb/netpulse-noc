#include "equipment_screen.h"

#include "app.h"
#include "screen.h"

#include "equipment_view.h"
#include "equipment_controller.h"

#include <stdlib.h>

typedef struct {
  screen_t parent;

  equipment_view_t        view;
  equipment_controller_t  controller;

} equipment_screen_t;


static screen_t *equipment_screen_create(app_t *app);


static const screen_descriptor_t descriptor = {
  .number     = SCREEN_EQUIPMENT,
  .text       = "Inventory",
  .stack_name = "INVENTORY",
  .create     = equipment_screen_create,
};


static void equipment_screen_destroy(screen_t *parent)
{
  equipment_screen_t *screen = (equipment_screen_t *)parent;

  equipment_view_destroy(&screen->view);

  free(screen);
}

static void equipment_screen_refresh(screen_t *parent)
{
  equipment_screen_t *screen = (equipment_screen_t *)parent;

  equipment_view_refresh(&screen->view);
}

static screen_t *equipment_screen_create(app_t *app)
{
  equipment_screen_t *screen = malloc(sizeof(*screen));
  if (screen == NULL) return NULL;

  equipment_controller_init(&screen->controller, &screen->view, &app->data);

  screen->parent.root = GTK_WIDGET(equipment_view_create(&screen->view, &screen->controller));

  screen->parent.refresh = equipment_screen_refresh;
  screen->parent.destroy = equipment_screen_destroy;

  return &screen->parent;
}

const screen_descriptor_t *equipment_screen_get_descriptor(void)
{
  return &descriptor;
}
