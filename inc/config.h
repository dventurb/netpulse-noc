#ifndef CONFIG_H
#define CONFIG_H

#include "macros.h"
#include <time.h>

typedef enum {
  CONFIG_IP_ADDRESS,
  CONFIG_MAC_ADDRESS,
  CONFIG_NAME,
  CONFIG_LOCATION,
  CONFIG_STATUS,
  CONFIG_VENDOR,
  CONFIG_MODEL,
  CONFIG_TYPE
} config_type_t;

typedef struct {
  int equipment_id; // TODO: Maybe change the equipment_id to a char (ex.: EQ-xxx)
  config_type_t config_type;
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


void config_stack_init(config_stack_t *stack);
void config_stack_destroy(config_stack_t *stack);
void config_stack_push(config_stack_t *stack, config_t data);
config_node_t *config_stack_pop(config_stack_t *stack);
config_node_t *config_stack_peek(config_stack_t *stack);

#endif
