#include "sensor.h"

#include <stdlib.h>

void sensor_list_init(sensor_list_t *list)
{
  list->head = NULL;
  list->count = 0;
}

void sensor_list_destroy(sensor_list_t *list)
{
  sensor_list_t *node = list->head;

  while (node != NULL)
  {
    sensor_list_t *next = node->next;
    free(node);
    node = next;
  }

  list->head = NULL;
  list->count = 0;
}

void sensor_list_insert(sensor_list_t *list, sensor_t data)
{
  sensor_node_t *new = malloc(sizeof(sensor_node_t));
  if (new == NULL) {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] sensor_list_insert : malloc failed)
    
    return;
  }

  new->data = data;

  new->next = list->head;
  list->head = new;

  list->count++;
}
