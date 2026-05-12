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

typedef incident_queue_t {
  incident_node_t *front;
  incident_node_t *rear;
  int count;
  int next_number; // TODO: Maybe change the number to a char (ex.: INC-xxx)
}

#endif
