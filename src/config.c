#include "config.h"

#include <stdlib.h>

void config_stack_push(config_stack_t *stack, config_t data)
{
  config_node_t *new = malloc(sizeof(config_node_t));
  if (new == NULL) 
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] config_stack_push : malloc failed)
    return;
  }

  new->data = data;

  if (stack->top == NULL) stack->count = 0;

  new->next = stack->top;
  stack->top = new;

  stack->count++;
}

config_node_t *config_stack_pop(config_stack_t *stack)
{
  if (stack->top == NULL) return;

  config_node_t *node = stack->top;
  stack->top = stack->top->next;

  stack->count--;

  return node;
}

config_node_t *config_stack_peek(config_stack_t *stack)
{
  if (stack->top == NULL) return;

  return stack->top;
}
