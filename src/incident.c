#include "incident.h"

#include <stdlib.h>
#include <string.h>

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
  list->head = NULL;
  list->count = 0;
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

    queue->count++;

    return;
  }

  new->data.number = queue->next_number++;

  queue->rear->next = new;
  queue->rear = new;

  queue->count++;
}

incident_node_t *incident_queue_dequeue(incident_queue_t *queue)
{
  if (queue == NULL || queue->front == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_queue_dequeue : NULL arguments)
    return NULL;
  }

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

incident_node_t *incident_queue_peek(incident_queue_t *queue)
{
  if (queue == NULL || queue->front == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_queue_peek : NULL arguments)
    return NULL;
  }

  return queue->front;
}


void incident_list_conclude(incident_node_t *node)
{
  if (node == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_list_conclude : NULL argument)
    return;
  }

  node->data.status = INCIDENT_CONCLUDED;
  node->data.concluded_at = time(NULL);
}

int incident_queue_filter_by_priority(const incident_queue_t *queue, incident_priority_t priority, incident_t *incidents)
{
  if (queue == NULL || incidents == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_queue_filter_by_priority : NULL arguments)
    return 0;
  }

  incident_node_t *node = queue->front;
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

  incident_node_t *node = queue->front;
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


int incident_queue_filter_by_source_id(const incident_queue_t *queue, const char *source_id, incident_t *incidents)
{
  if (queue == NULL || source_id == NULL || incidents)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_queue_filter_by_source_id : NULL arguments)
    return 0;
  }

  incident_node_t *node = queue->front;
  int i = 0;

  while (node != NULL && i < queue->count)
  {
    if (strcmp(node->data.source_id, source_id) == 0)
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

  while (node != NULL && i < list->count)
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
  if (list == NULL || incidents == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_list_filter_by_status : NULL arguments)
    return 0;
  }

  incident_node_t *node = list->head;
  int i = 0;

  while (node != NULL && i < list->count)
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

int incident_list_filter_by_source_id(const incident_list_t *list, const char *source_id, incident_t *incidents)
{
  if (list == NULL || source_id == NULL || incidents == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] incident_list_filter_by_source_id : NULL arguments)
    return 0;
  }

  incident_node_t *node = list->head;
  int i = 0;

  while (node != NULL && list->count)
  { 
    if (strcmp(node->data.source_id, source_id) == 0)
    {
      incidents[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}

int incident_get_count(incident_queue_t *queue, incident_list_t *list)
{
  return incident_queue_get_count(queue) + incident_list_get_count(list);
}

int incident_queue_get_count(incident_queue_t *queue)
{
  return queue->count;
}

int incident_list_get_count(incident_list_t *list)
{
  return list->count;
}

int incident_list_get_number_status(incident_list_t *list, incident_status_t status)
{
  int i = 0;

  incident_node_t *node = list->head;

  while (node != NULL)
  {
    if (node->data.status == status) i++;

    node = node->next;
  }

  return i;
}

const char *incident_priority_to_string(incident_priority_t priority)
{
  switch (priority) 
  {
    case PRIORITY_LOW: return "LOW";
    case PRIORITY_MEDIUM: return "MEDIUM";
    case PRIORITY_HIGH: return "HIGH";
    case PRIORITY_CRITICAL: return "CRITICAL";
  }
}

const char *incident_status_to_string(incident_status_t status)
{
  switch (status) 
  {
    case INCIDENT_PENDING: return "Pending";
    case INCIDENT_IN_PROGRESS: return "In Progress";
    case INCIDENT_CONCLUDED: return "Concluded";
  }
}

