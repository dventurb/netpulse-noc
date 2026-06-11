#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "macros.h"
#include <time.h>

typedef struct {
  int number;
  char command[COMMAND_MAX];
  char technician_name[STRING_MAX];
  time_t operated_at;
} configuration_t;

typedef struct configuration_node_t {
  configuration_t data;
  struct configuration_node_t *next;
} configuration_node_t;

typedef struct {
  configuration_node_t *top;
  int next_number;
  int count;
} configuration_stack_t;

void configuration_stack_init(configuration_stack_t *stack);
void configuration_stack_destroy(configuration_stack_t *stack);

void configuration_stack_push(configuration_stack_t *stack, configuration_t data);
configuration_node_t *configuration_stack_pop(configuration_stack_t *stack);
configuration_node_t *configuration_stack_peek(configuration_stack_t *stack);

void configuration_stack_clone(configuration_stack_t *source, configuration_stack_t *destination);
configuration_t *configuration_stack_in_range(configuration_stack_t *stack, int start, int end, int *count);

int configuration_get_count(configuration_stack_t *stack);

#endif
