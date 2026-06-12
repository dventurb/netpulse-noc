#include "configuration.h"

#include <stdio.h>
#include <stdlib.h>

void configuration_stack_init(configuration_stack_t *stack)
{
  stack->top = NULL;
  stack->next_number = 1;
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
  stack->next_number = 0;
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
  new->data.number = stack->next_number++;
  new->data.operated_at = time(NULL);

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

void configuration_stack_repush(configuration_stack_t *stack, configuration_t data)
{
  configuration_node_t *new = malloc(sizeof(configuration_node_t));
  if (new == NULL) 
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] configuration_stack_repush : malloc failed)
    return;
  }

  new->data = data;
  if (data.number >= stack->next_number) stack->next_number = data.number + 1;

  new->next = stack->top;
  stack->top = new;

  stack->count++;
}

void configuration_stack_clone(configuration_stack_t *source, configuration_stack_t *destination)
{
  if (source == NULL || destination == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] configuration_stack_clone : NULL arguments)
    return;
  }

  if (source->count == 0) return;

  configuration_t *temp = malloc(sizeof(configuration_t) *source->count);
  if (temp == NULL) return;

  configuration_node_t *node = source->top;
  int i = 0;

  while (node != NULL && i < source->count)
  {
    temp[i] = node->data;
    i++;
    node = node->next;
  }

  for (int j = i - 1; j >= 0; j--) 
  {
    configuration_stack_repush(destination, temp[j]);
  }

  free(temp);
}

configuration_t *configuration_stack_in_range(configuration_stack_t *stack, int start, int end, int *count)
{
  if (stack == NULL || stack->count == 0)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] configuration_stack_in_range : NULL argument)
    return NULL;
  }

  int size = end - start;
  if (size <= 0) return NULL;

  configuration_t *configs = malloc(sizeof(configuration_t) * size);
  if (configs == NULL) return NULL;

  configuration_node_t *node = stack->top;

  int i = 0;

  while (node != NULL && i < start)
  {
    node = node->next;
    i++;
  }

  int j = 0;

  while (node != NULL && i < end)
  {
    configs[j++] = node->data;

    node = node->next;
    i++;
  }

  *count = j;

  return configs;
}

int configuration_get_count(configuration_stack_t *stack)
{
  return stack->count;
}
