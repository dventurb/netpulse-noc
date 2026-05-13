#include "equipment.h"

#include <stdlib.h>

void equipment_list_init(equipment_list_t *list)
{
  list->head = NULL;
  list->tail = NULL;
  list->count = 0;
  list->next_id = 1;
}

void equipment_list_destroy(equipment_list_t *list)
{
  equipment_node_t *node = list->head;
  
  while (node != NULL)
  {
    equipment_node_t *next = node->next;
    free(node);
    node = next;
  }

  list->head = NULL;
  list->tail = NULL;
  list->count = 0;
  list->next_id = 0;
}

void equipment_list_insert(equipment_list_t *list, equipment_t data)
{
  equipment_node_t *new = malloc(sizeof(equipment_node_t));
  if (new == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] equipment_list_insert : malloc failed)
    return;
  }

  new->data = data;
  new->next = NULL;

  if (list->head == NULL)
  {
    new->data.id = list->next_id++;
    new->previous = NULL;

    list->head = new;
    list->tail = new;

    list->count++;

    return;
  }
  
  new->data.id = list->next_id++;

  new->previous = list->tail;
  list->tail->next = new;
  list->tail = new;

  list->count++;
}

void equipment_list_remove(equipment_list_t *list, equipment_node_t *node) 
{
  if (list->head == NULL || node == NULL) return;
  
  if (node != list->head) node->previous->next = node->next;

  else list->head = node->next;

  if (node->next != NULL) node->next->previous = node->previous;

  else list->tail = node->previous;

  free(node);

  list->count--;
}
