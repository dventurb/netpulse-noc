#include "connectivity_controller.h"

#include "connectivity_view.h"

void connectivity_controller_init(connectivity_controller_t *controller, connectivity_view_t *view, app_data_t *data)
{
  controller->view = view;
  controller->data = data;
}
