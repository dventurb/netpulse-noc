#ifndef SENSOR_H
#define SENSOR_H

#include "macros.h"
#include <stdio.h>
#include <stdbool.h>
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

typedef struct {
  sensor_t *sensors;
  int count;
} sensor_array_t;


void sensor_array_init(sensor_array_t *array);
void sensor_array_destroy(sensor_array_t *array);

void sensor_persistence_append(sensor_t *sensor);

void sensor_array_clone(sensor_array_t *source, sensor_array_t *destination);

sensor_t *sensor_array_in_range(sensor_array_t *array, int start, int end, int *count);

void sensor_search_by_date(sensor_array_t *array, time_t date);

void sensor_filter_by_status(const sensor_array_t *array, sensor_status_t status, sensor_array_t *filtered);
void sensor_filter_by_code(const sensor_array_t *array, const char *code, sensor_array_t *filtered);

int sensor_get_count(sensor_array_t array);
int sensor_get_number_code(sensor_array_t array, const char *code);
int sensor_get_number_status(sensor_array_t array, sensor_status_t status);

bool sensor_validate(sensor_t sensor);
sensor_t sensor_create_from_line(char *line);

time_t sensor_format_timestamp(const char *timestamp);

sensor_status_t sensor_string_to_status(const char *string);
const char *sensor_status_to_string(sensor_status_t status);

#endif
