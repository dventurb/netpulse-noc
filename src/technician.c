#include "technician.h"

#include <stdlib.h>


void technician_list_insert(technician_list_t *list, technician_t data)
{
  technician_node_t *new = malloc(sizeof(technician_node_t));
  if (new == NULL) 
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] technician_list_insert : malloc failed)
    return;
  }

  new->data = data;
  new->previous = NULL;

  if (list->head == NULL)
  {
    list->count = 1;
    list->next_id = 1;

    new->data.id = list->next_id++;
    new->next = NULL;

    list->head = new;

    return;
  }

  new->data.id = list->next_id++;

  new->next = list->head;
  list->head->previous = new;
  list->head = new;

  list->count++;
}

void technician_list_remove(technician_list_t *list, technician_node_t *node)
{
  if (list->head == NULL || node == NULL) return;

  if (node != list->head) node->previous->next = node->next;

  else list->head = node->next;

  if (node->next != NULL) node->next->previous = node->previous;

  free(node);

  list->count--;
}
