#ifndef CONNECTIVITY_CONTROLLER_H
#define CONNECTIVITY_CONTROLLER_H 

#include <glib.h>

#include "app_data.h"

// forward declaration to resolve circular dependencies
typedef struct connectivity_view_t connectivity_view_t;

typedef struct {

  app_data_t *data;

  connectivity_view_t *view;

} connectivity_controller_t;


void connectivity_controller_init(connectivity_controller_t *controller, connectivity_view_t *view, app_data_t *data);

#endif
