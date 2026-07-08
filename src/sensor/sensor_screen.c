#include "sensor_screen.h"

#include "app.h"
#include "screen.h"

#include "sensor_view.h"
#include "sensor_controller.h"

#include <stdlib.h>


typedef struct {
  screen_t parent;

  sensor_view_t        view;
  sensor_controller_t  controller;

} sensor_screen_t;


static screen_t *sensor_screen_create(app_t *app);


static const screen_descriptor_t descriptor = {
  .number     = SCREEN_SENSOR,
  .text       = "Sensors",
  .stack_name = "SENSOR",
  .create     = sensor_screen_create,
};


static void sensor_screen_destroy(screen_t *parent)
{
  sensor_screen_t *screen = (sensor_screen_t *)parent;

  sensor_view_destroy(&screen->view);

  free(screen);
}

static void sensor_screen_refresh(screen_t *parent)
{
  sensor_screen_t *screen = (sensor_screen_t *)parent;

  sensor_view_refresh(&screen->view);
}

static screen_t *sensor_screen_create(app_t *app)
{
  sensor_screen_t *screen = malloc(sizeof(*screen));
  if (screen == NULL) return NULL;

  sensor_controller_init(&screen->controller, &screen->view, &app->data);

  screen->parent.root = GTK_WIDGET(sensor_view_create(&screen->view, &screen->controller));

  screen->parent.refresh = sensor_screen_refresh;
  screen->parent.destroy = sensor_screen_destroy;

  return &screen->parent;
}

const screen_descriptor_t *sensor_screen_get_descriptor(void)
{
  return &descriptor;
}
