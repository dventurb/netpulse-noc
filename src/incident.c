#include "incident.h"

#include <stdlib.h>

void incident_queue_init(incident_queue_t *queue)
{
  queue->front = NULL;
  queue->rear = NULL;
  queue->count = 0;
  queue->next_number = 1;
}

void incident_queue_destroy(incident_queue_t *queue)
{
  incident_node_t *node = queue->front;

  while (node != NULL)
  {
    incident_node_t *next = node->next;
    free(node);
    node = next;
  }

  queue->front = NULL;
  queue->rear = NULL;
  queue->count = 0;
  queue->next_number = 0;
}

void incident_list_init(incident_list_t *list)
{
  queue->head = NULL;
  queue->count = 0;
}

void incident_list_destroy(incident_list_t *list)
{
  incident_node_t *node = list->head;

  while (node != NULL) 
  {
    incident_node_t *next = node->next;
    free(node);
    node = next;
  }

  list->head = NULL;
  list->count = 0;
}

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
    new->data.number = queue->next_number++;

    queue->front = new;
    queue->rear = new;

    list->count++

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
  node->next = list->head;
  list->head = node;

  list->count++;
}

int incident_queue_filter_by_priority(const incident_queue_t *queue, incident_priority_t priority, incident_t *incidents)
{
  if (queue == NULL || incidents == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_queue_filter_by_priority : NULL arguments)
    return 0;
  }

  incident_node_t *node = queue->head;
  int i = 0;

  while (node != NULL && i < queue->count)
  {
    if (node->data.priority == priority)
    {
      incidents[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}

int incident_queue_filter_by_status(const incident_queue_t *queue, incident_status_t status, incident_t *incidents)
{
  if (queue == NULL || incidents == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_queue_filter_by_status : NULL arguments)
    return 0;
  }

  incident_node_t *node = queue->head;
  int i = 0;

  while (node != NULL && i < queue->count)
  {
    if (node->data.status == status)
    {
      incidents[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}

int incident_list_filter_by_priority(const incident_list_t *list, incident_priority_t priority, incident_t *incidents)
{
  if (list == NULL || incidents == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_list_filter_by_priority : NULL arguments)
    return 0;
  }

  incident_node_t *node = list->head;
  int i = 0;

  while (node != NULL)
  {
    if (node->data.priority == priority)
    {
      incidents[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return 1;
}

int incident_list_filter_by_status(const incident_list_t *list, incident_status_t status, incident_t *incidents)
{
  if (list == NULL || incidets == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_list_filter_by_status : NULL arguments)
    return 0;
  }

  incident_node_t *node = list->head;
  int i = 0;

  while (node != NULL)
  {
    if (node->data.status == status)
    {
      incidents[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}
