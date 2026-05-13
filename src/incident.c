#include "incident.h"

#include <stdlib.h>

void incident_queue_enqueue(incident_queue_t *queue, incident_t data)
{
  incident_node_t *new = malloc(sizeof(incident_node_t));
  if (new == NULL) 
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_queue_enqueue : malloc failed)
    return;
  }

  new->data = data;
  new->next = NULL;

  if (queue->front == NULL)
  {
    queue->count = 1;
    queue->next_number = 1;

    new->data.number = queue->next_number++;

    queue->front = new;
    queue->rear = new;

    return;
  }

  new->data.number = queue->next_number++;

  queue->rear->next = new;
  queue->rear = new;

  queue->count++;
}

incident_node_t *incident_queue_dequeue(incident_queue_t *queue)
{
  if (queue->front == NULL) return;

  incident_node_t *node = queue->front;
  
  queue->front = queue->front->next;
  if (queue->front == NULL) queue->rear = NULL;

  queue->count--;

  return node;
}

void incident_list_insert(incident_list_t *list, incident_node_t *node)
{
  if (list->head == NULL) list->count = 0;

  node->next = list->head;
  list->head = node->next;

  list->count++;
}
