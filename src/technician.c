#include "technician.h"

#include <stdlib.h>

void technician_list_init(technician_list_t *list)
{
  list->head = NULL;
  list->count = 0;
  list->next_id = 1;
}

void technician_list_destroy(technician_list_t *list)
{
  technician_node_t *node = list->head;

  while (node != NULL)
  {
    technician_node_t *next = node->next;
    free(node);
    node = next;
  }

  list->head = NULL;
  list->count = 0;
  list->next_id = 0;
}

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
    new->data.id = list->next_id++;
    new->next = NULL;

    list->head = new;

    list->count++;

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

int technician_filter_by_role(technician_list_t *list, technician_role_t role, technician_t *technicians)
{
  if (list == NULL || technicians == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] technician_filter_by_role : NULL arguments)

    return 0;
  }

  technician_node_t *node = list->head;
  int i = 0;

  while (node != NULL && i < list->count)
  {
    if (node->data.role == role)
    {
      technicians[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}
