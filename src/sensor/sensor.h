#ifndef SENSOR_H
#define SENSOR_H

#include "macros.h"
#include <time.h>
#include <stdbool.h>

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
int sensor_filter_by_code(const sensor_list_t *list, const char *code, sensor_t *sensors);

bool sensor_validate(sensor_t sensor);
sensor_t sensor_create_from_line(char *line);
void sensor_insert_from_file(sensor_list_t *list, const char *filepath);

time_t sensor_format_timestamp(const char *timestamp);

sensor_status_t sensor_string_to_status(const char *string);
const char *sensor_status_to_string(sensor_status_t status);

#endif
