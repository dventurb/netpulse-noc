#include "configuration_controller.h"

#include "configuration_worker.h"
#include "configuration_view.h"

#include "persistence.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void configuration_controller_init(configuration_controller_t *controller, configuration_view_t *view, void *data)
{
  controller->view = view;
  controller->app = (application_t *)data;

  controller->search_text[0] = '\0';

  controller->pagination.page = 0;
  controller->pagination.page_size = 6;
  //controller->pagination.total = configuration_get_count(&controller->app->equipments.configs);
}
