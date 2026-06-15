#include "sensor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "macros.h"

static const char *filepath = "data/sensors.bin";

static void binary_search(FILE *file, time_t date, int total, int *start, int *end);
static void merge_sort(sensor_t *array, int size);
static void merge(sensor_t *array, sensor_t *left, sensor_t *right, int nl, int nr);

void sensor_array_init(sensor_array_t *array)
{
  array->sensors = NULL;
  array->count = 0;
}

void sensor_array_destroy(sensor_array_t *array)
{
  if (array->sensors != NULL) free(array->sensors);
  array->count = 0;
}

void sensor_persistence_append(sensor_t *sensor)
{
  if (sensor == NULL) return;

  FILE *file = fopen(filepath, "ab");
  if (file == NULL) return;

  fwrite(sensor, sizeof(sensor_t), 1, file);

  fclose(file);
}

void sensor_persistence_sort(void)
{
  FILE *file = fopen(filepath, "rb");
  if (file == NULL) return;

  fseek(file, 0 , SEEK_END);
  long size = ftell(file);

  int count = (int)size / sizeof(sensor_t);
  if (count <= 1) 
  {
    fclose(file);
    return;
  }

  sensor_t *sensors = malloc(sizeof(sensor_t) * count);
  if (sensors == NULL)
  {
    fclose(file);
    return;
  }

  fseek(file, 0, SEEK_SET);

  fread(sensors, sizeof(sensor_t), count, file);

  fclose(file);

  merge_sort(sensors, count);

  file = fopen(filepath, "wb");
  if (file == NULL)
  {
    free(sensors);
    return;
  }

  fwrite(sensors, sizeof(sensor_t), count, file);

  fclose(file);

  free(sensors);
}

void sensor_array_clone(sensor_array_t *source, sensor_array_t *destination)
{
  if (source == NULL || destination == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] sensor_array_clone : NULL arguments)
    return;
  }

  if (source->count == 0) return;

  destination->sensors = malloc(sizeof(sensor_t) * source->count);
  if (destination->sensors == NULL) return;

  for (int i = 0; i < source->count; i++) 
  {
    destination->sensors[i] = source->sensors[i];
    destination->count++;
  }
}

sensor_t *sensor_array_in_range(sensor_array_t *array, int start, int end, int *count)
{
  if (array == NULL || array->sensors == NULL) 
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] sensor_array_in_range : NULL argument)
    return NULL;
  }

  if (start < 0) start = 0;
  if (end > array->count) end = array->count;
  if (start > end) start = end;

  int size = end - start;
  if (size <= 0) return NULL;

  sensor_t *sensors = malloc(sizeof(sensor_t) * size);
  if (sensors == NULL) return NULL;

  int i = 0;

  for (int j = start; j < end; j++)
  {
    sensors[i++] = array->sensors[j];
  }

  *count = i;

  return sensors;
}

void sensor_search_by_date(sensor_array_t *array, time_t date)
{
  FILE *file = fopen(filepath, "rb");
  if (file == NULL) return;

  char datime[DATETIME_MAX];
  format_timestamp_to_datetime(date, datime);

  fseek(file, 0, SEEK_END);
  long size = ftell(file);

  int total = (int)size / sizeof(sensor_t);
  if (total <= 0) 
  {
    fclose(file);
    return;
  }

  int start = -1, end = -1;
  binary_search(file, date, total, &start, &end);

  if (start < 0 || end < 0 || end < start || end >= total) 
  {
    array->sensors = NULL;
    array->count = 0;
    fclose(file);
    return;
  }

  int count = (end - start) + 1;

  if (array->sensors != NULL) 
    array->sensors = NULL;

  array->sensors = malloc(sizeof(sensor_t) * count);
  if (array->sensors == NULL)
  {
    fclose(file);
    return;
  }

  fseek(file, start * sizeof(sensor_t), SEEK_SET);
  fread(array->sensors, sizeof(sensor_t), count, file);
  
  array->count = count;

  fclose(file);
}

void sensor_filter_by_status(const sensor_array_t *array, sensor_status_t status, sensor_array_t *filtered)
{
  if (array == NULL || filtered == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] sensor_filter_status : NULL arguments)
    return;
  }

  int count = sensor_get_number_status(*array, status);
  if (count == 0) return;
 
  filtered->sensors = malloc(sizeof(sensor_t) * count);
  if (filtered->sensors == NULL) return;

  int i = 0;

  for (int j = 0; j < array->count; j++) 
  {
    if (array->sensors[j].status == status)
      filtered->sensors[i++] = array->sensors[j];
  }

  filtered->count = i;
}

void sensor_filter_by_code(const sensor_array_t *array, const char *code, sensor_array_t *filtered){
  if (array == NULL || code == NULL || filtered == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] sensor_filter_by_code : NULL arguments)
    return;
  }

  int count = sensor_get_number_code(*array, code);
  if (count == 0) return;

  filtered->sensors = malloc(sizeof(sensor_t) * count);
  if (filtered->sensors == NULL) return;

  int i = 0;

  for (int j = 0; j < array->count; j++) 
  {
    if (strncmp(array->sensors[j].code, code, strlen(code)) == 0)
      filtered->sensors[i++] = array->sensors[j];
  }

  filtered->count = i;
}

int sensor_get_count(sensor_array_t array)
{
  return array.count;
}

int sensor_get_number_code(sensor_array_t array, const char *code)
{
  int i = 0;

  for (int j = 0; j < array.count; j++) 
  {
    if (strncmp(array.sensors[j].code, code, strlen(code)) == 0) i++;
  }

  return i;
}

int sensor_get_number_status(sensor_array_t array, sensor_status_t status)
{
  int i = 0;

  for (int j = 0; j < array.count; j++) 
  {
    if (array.sensors->status == status) i++;
  }

  return i;
}

bool sensor_validate(sensor_t sensor)
{
  if (!validate_sensor_code(sensor.code)) return false;
  if (strlen(sensor.type) <= 6) return false;
  if (sensor.value < -100 || sensor.value > 10000) return false;
  if (strlen(sensor.unit) < 1) return false;
  if (sensor.status < SENSOR_OK || sensor.status > SENSOR_NET_FAILURE) return false;
  else return true;
}

sensor_t sensor_create_from_line(char *line)
{
  sensor_t sensor = {0};
  char *saveptr; // thread safe instead of a static pointer by strtok
  char *token = strtok_r(line, ";", &saveptr); // ';' as delimiter
  
  int column = 0;

  while (token != NULL)
  {
    ++column;
    
    if (column == 1) sensor.read_at = sensor_format_timestamp(token);
    else if (column == 5) snprintf(sensor.code, CODE_MAX, "%s", token);
    else if (column == 6) snprintf(sensor.type, STRING_MAX, "%s", token);
    else if (column == 8) sscanf(token, "%f", &sensor.value);
    else if (column == 9) snprintf(sensor.unit, UNIT_MAX, "%s", token);
    else if (column == 10) sensor.status = sensor_string_to_status(token);

    token = strtok_r(NULL, ";", &saveptr);
  }

  return sensor;
}

time_t sensor_format_timestamp(const char *timestamp)
{
  struct tm tm;

  // 2026-06-09T16:34:00
  sscanf(timestamp, "%4d-%2d-%2dT%2d:%2d:%2d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

  tm.tm_year -= 1900; // years since 1900
  tm.tm_mon -= 1; // January is 0
  
  time_t time = mktime(&tm);

  return time;
}

sensor_status_t sensor_string_to_status(const char *string)
{
  if (strcmp(string, "NORMAL") == 0) return SENSOR_OK;
  if (strcmp(string, "AVISO") == 0) return SENSOR_WARNING;
  if (strcmp(string, "CRITICO") == 0) return SENSOR_CRITICAL;
  if (strcmp(string, "FALHA_REDE") == 0) return SENSOR_NET_FAILURE;
  else return SENSOR_WARNING;
}

const char *sensor_status_to_string(sensor_status_t status)
{
  switch (status) 
  {
    case SENSOR_OK: return "OK";
    case SENSOR_WARNING: return "Warning";
    case SENSOR_CRITICAL: return "Critical";
    case SENSOR_NET_FAILURE: return "Network Failure";
    default: return "Unknown";
  }
}

static void binary_search(FILE *file, time_t date, int total, int *start, int *end)
{
  time_t date_start = get_day_start_timestamp(date);
  time_t date_end = get_day_end_timestamp(date);

  int left = 0;
  int right = total - 1;

  while (left <= right)
  {
    int mid = left + (right - left) / 2;

    sensor_t sensor = {0};

    fseek(file, mid * sizeof(sensor_t), SEEK_SET);
    fread(&sensor, sizeof(sensor_t), 1, file);

    time_t sensor_date = get_day_start_timestamp(sensor.read_at);

    if (sensor_date >= date_start)
    {
      *start = mid;
      right = mid - 1;
    }
    
    else left = mid + 1;
  }

  if (start == 0) return;

  left = 0;
  right = total - 1;

  while (left <= right)
  {
    int mid = left + (right - left) / 2;

    sensor_t sensor = {0};

    fseek(file, mid * sizeof(sensor_t), SEEK_SET);
    fread(&sensor, sizeof(sensor_t), 1, file);

    time_t sensor_date = get_day_end_timestamp(sensor.read_at);

    if (sensor_date <= date_end)
    {
      *end = mid;
      left = mid + 1;
    }

    else right = mid - 1;
  }
}

static void merge_sort(sensor_t *array, int size)
{
  if (size < 2) return;

  int mid = size / 2;

  sensor_t *left = malloc(sizeof(sensor_t) * mid);
  if (left == NULL) return;

  sensor_t *right = malloc(sizeof(sensor_t) * (size - mid));
  if (right == NULL) return;

  for (int i = 0; i < mid; i++) 
    left[i] = array[i];

  for (int i = mid; i < size; i++) 
    right[i - mid] = array[i];

  merge_sort(left, mid);
  merge_sort(right, size - mid);

  merge(array, left, right, mid, size - mid);

  free(left);
  free(right);
}

static void merge(sensor_t *array, sensor_t *left, sensor_t *right, int nl, int nr)
{
  int i = 0, j = 0, k = 0;

  while (i < nl && j < nr)
  {
    if (left[i].read_at < right[j].read_at)
      array[k++] = left[i++];
    else 
      array[k++] = right[j++];
  }

  while (i < nl)
    array[k++] = left[i++];

  while (j < nr)
    array[k++] = right[j++];
}
