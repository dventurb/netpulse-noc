#ifndef SENSOR_H
#define SENSOR_H

#include "macros.h"
#include <time.h>

typedef enum {
  SENSOR_OK,
  SENSOR_WARNING,
  SENSOR_CRITICAL,
  SENSOR_NET_FAILURE
} sensor_status_t;

typedef struct {
  char code[CODE_MAX];
  char type[STRING_MAX];
  float value;
  char unit[UNIT_MAX];
  sensor_status_t status;
  time_t read_at;
} sensor_t;

typedef struct sensor_node_t {
  sensor_t data;
  struct sensor_node_t *next;
} sensor_node_t;

typedef struct {
  sensor_node_t *head;
  int count;
} sensor_list_t;


void sensor_list_init(sensor_list_t *list);
void sensor_list_destroy(sensor_list_t *list);
void sensor_list_insert(sensor_list_t *list, sensor_t data);

int sensor_filter_status(const sensor_list_t *list, sensor_status_t status, sensor_t *sensors);

#endif
