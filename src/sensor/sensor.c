#include "sensor.h"

#include <stdlib.h>
#include <string.h>

void sensor_list_init(sensor_list_t *list)
{
  list->head = NULL;
  list->count = 0;
}

void sensor_list_destroy(sensor_list_t *list)
{
  sensor_node_t *node = list->head;

  while (node != NULL)
  {
    sensor_node_t *next = node->next;
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

int sensor_filter_status(const sensor_list_t *list, sensor_status_t status, sensor_t *sensors)
{
  if (list == NULL || sensors == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] sensor_filter_status : NULL arguments)
    return 0;
  }

  sensor_node_t *node = list->head;
  int i = 0;

  while (node != NULL && i < list->count)
  {
    if (node->data.status == status)
    {
      sensors[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}


int sensor_filter_by_code(const sensor_list_t *list, const char *code, sensor_t *sensors){
  if (list == NULL || code == NULL || sensors == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] sensor_filter_by_code : NULL arguments)
    return 0;
  }

  sensor_node_t *node = list->head;
  int i = 0;

  while (node != NULL && i < list->count)
  {
    if (strcmp(node->data.code, code) == 0)
    {
      sensors[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}
