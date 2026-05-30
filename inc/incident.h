#ifndef INCIDENT_H
#define INCIDENT_H

#include "macros.h"
#include <time.h>

typedef enum {
  PRIORITY_LOW,
  PRIORITY_MEDIUM,
  PRIORITY_HIGH,
  PRIORITY_CRITICAL
} incident_priority_t;

typedef enum {
  INCIDENT_PENDING,
  INCIDENT_IN_PROGRESS,
  INCIDENT_CONCLUDED
} incident_status_t;

typedef enum {
  SOURCE_EQUIPMENT,
  SOURCE_SENSOR
} incident_source_t;

typedef struct {
  int number; // TODO: Maybe change the number to a char (ex.: INC-xxx)
  char source_id[STRING_MAX];
  incident_source_t source_type;
  char type[STRING_MAX];
  char description[DESCRIPTION_MAX];
  incident_priority_t priority;
  incident_status_t status;
  char technician_name[STRING_MAX];
  time_t created_at;
  time_t concluded_at;
} incident_t;

typedef struct incident_node_t {
  incident_t data;
  struct incident_node_t *next;
} incident_node_t;

typedef struct {
  incident_node_t *front;
  incident_node_t *rear;
  int count;
  int next_number; // TODO: Maybe change the number to a char (ex.: INC-xxx)
} incident_queue_t;

typedef struct {
  incident_node_t *head;
  int count;
} incident_list_t;

void incident_queue_init(incident_queue_t *queue);
void incident_queue_destroy(incident_queue_t *queue);
void incident_list_init(incident_list_t *list);
void incident_list_destroy(incident_list_t *list);
void incident_queue_enqueue(incident_queue_t *queue, incident_t data);
incident_node_t *incident_queue_dequeue(incident_queue_t *queue); // Flow: dequeue() -> return incident_node_t * -> node->data.status = INCIDENT_IN_PROGRESS -> incident_list_insert()
void incident_list_insert(incident_list_t *list, incident_node_t *node);
incident_node_t *incident_queue_peek(incident_queue_t *queue);
void incident_list_conclude(incident_node_t *node);

int incident_queue_filter_by_priority(const incident_queue_t *queue, incident_priority_t priority, incident_t *incidents);
int incident_queue_filter_by_status(const incident_queue_t *queue, incident_status_t status, incident_t *incidents);
int incident_queue_filter_by_source_id(const incident_queue_t *queue, const char *source_id, incident_t *incidents);
int incident_list_filter_by_priority(const incident_list_t *list, incident_priority_t priority, incident_t *incidents);
int incident_list_filter_by_status(const incident_list_t *list, incident_status_t status, incident_t *incidents);
int incident_list_filter_by_source_id(const incident_list_t *list, const char *source_id, incident_t *incidents);

int incident_get_count(incident_queue_t *queue, incident_list_t *list);
int incident_queue_get_count(incident_queue_t *queue);
int incident_list_get_count(incident_list_t *list);
int incident_list_get_number_status(incident_list_t *list, incident_status_t status);

const char *incident_priority_to_string(incident_priority_t priority);
const char *incident_status_to_string(incident_status_t status);

#endif
