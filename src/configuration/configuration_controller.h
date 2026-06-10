#ifndef CONFIGURATION_CONTROLLER_H
#define CONFIGURATION_CONTROLLER_H

#include <glib.h>

#include "application.h"
#include "configuration.h"
#include "pagination.h"
#include "macros.h"

// forward declaration to resolve circular dependencies
typedef struct configuration_view_t configuration_view_t;
typedef struct configuration_task_t configuration_task_t;

typedef struct {
  application_t *app;

  configuration_view_t *view;

  char search_text[STRING_MAX];

  pagination_t pagination;
} configuration_controller_t;

typedef struct {
  int total;
  int technicians;
} configuration_stats_t;

void configuration_controller_init(configuration_controller_t *controller, configuration_view_t *view, void *data);

#endif

