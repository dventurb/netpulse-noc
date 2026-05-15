#include "config.h"

#include <stdlib.h>

void config_stack_init(config_stack_t *stack)
{
  stack->top = NULL;
  stack->count = 0;
}

void config_stack_destroy(config_stack_t *stack)
{
  config_node_t *node = stack->top;

  while (node != NULL)
  {
    config_node_t *next = node->next;
    free(node);
    node = next;
  }

  stack->top = NULL;
  stack->count = 0;
}

void config_stack_push(config_stack_t *stack, config_t data)
{
  config_node_t *new = malloc(sizeof(config_node_t));
  if (new == NULL) 
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] config_stack_push : malloc failed)
    return;
  }

  new->data = data;

  new->next = stack->top;
  stack->top = new;

  stack->count++;
}

config_node_t *config_stack_pop(config_stack_t *stack)
{
  if (stack == NULL || stack->top == NULL) 
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] config_stack_pop : NULL arguments)
    return NULL;
  }

  config_node_t *node = stack->top;
  stack->top = stack->top->next;

  stack->count--;

  return node;
}

config_node_t *config_stack_peek(config_stack_t *stack)
{
  if (stack == NULL || stack->top == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] config_stack_peek : NULL arguments)
    return NULL;
  }

  return stack->top;
}

int config_filter_by_equipment(const config_stack_t *stack, int equipment_id, config_t *configs)
{
  if (stack == NULL || configs == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] config_filter_by_equipment : NULL arguments)
    return 0;
  }

  config_node_t *node = stack->top;
  int i = 0;

  while (node != NULL && i < stack->count)
  {
    if (node->data.equipment_id == equipment_id)
    {
      configs[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}
