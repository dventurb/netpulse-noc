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

  node *node = queue->front;
  
  queue->front = queue->front->next;
  if (queue->front == NULL) queue->rear = NULL;

  return node;
}

void incident_list_insert(incident_list_t *list, incident_node_t *node)
{
  node->next = list->head;
  list->head = node->next;
}
