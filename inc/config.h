#ifndef CONFIG_H
#define CONFIG_H

#include "macros.h"
#include <time.h>

typedef struct {
  int equipment_id; // TODO: Maybe change the equipment_id to a char (ex.: EQ-xxx)
  char config_type[STRING_MAX];
  char previous_values[STRING_MAX];
  char new_values[STRING_MAX];
  char technician_name[STRING_MAX];
  time_t operated_at;
} config_t;

typedef struct config_node_t {
  config_t data;
  struct config_node_t *next;
} config_node_t;

typedef struct {
  config_node_t *top;
  int count;
} config_stack_t;

#endif
