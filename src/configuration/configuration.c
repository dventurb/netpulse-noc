#include "configuration.h"

#include <stdlib.h>

void configuration_stack_init(configuration_stack_t *stack)
{
  stack->top = NULL;
  stack->count = 0;
}

void configuration_stack_destroy(configuration_stack_t *stack)
{
  configuration_node_t *node = stack->top;

  while (node != NULL)
  {
    configuration_node_t *next = node->next;
    free(node);
    node = next;
  }

  stack->top = NULL;
  stack->count = 0;
}

void configuration_stack_push(configuration_stack_t *stack, configuration_t data)
{
  configuration_node_t *new = malloc(sizeof(configuration_node_t));
  if (new == NULL) 
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] configuration_stack_push : malloc failed)
    return;
  }

  new->data = data;

  new->next = stack->top;
  stack->top = new;

  stack->count++;
}

configuration_node_t *configuration_stack_pop(configuration_stack_t *stack)
{
  if (stack == NULL || stack->top == NULL) 
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] configuration_stack_pop : NULL arguments)
    return NULL;
  }

  configuration_node_t *node = stack->top;
  stack->top = stack->top->next;

  stack->count--;

  return node;
}

configuration_node_t *configuration_stack_peek(configuration_stack_t *stack)
{
  if (stack == NULL || stack->top == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] configuration_stack_peek : NULL arguments)
    return NULL;
  }

  return stack->top;
}

int configuration_get_count(configuration_stack_t *stack)
{
  return stack->count;
}
