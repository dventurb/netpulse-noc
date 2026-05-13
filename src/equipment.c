#include "equipment.h"

#include <stdlib.h>

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
    list->count = 1;
    list->next_id = 1;

    new->data.id = list->next_id++;
    new->previous = NULL;

    list->head = new;
    list->tail = new;

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
