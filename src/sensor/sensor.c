#include "sensor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "macros.h"

void sensor_list_init(sensor_list_t *list)
{
  list->head = NULL;
  list->count = 0;
}

void sensor_list_destroy(sensor_list_t *list)
{
  sensor_node_t *node = list->head;

  while (node != NULL)
  {
    sensor_node_t *next = node->next;
    free(node);
    node = next;
  }

  list->head = NULL;
  list->count = 0;
}

void sensor_list_insert(sensor_list_t *list, sensor_t data)
{
  sensor_node_t *new = malloc(sizeof(sensor_node_t));
  if (new == NULL) {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] sensor_list_insert : malloc failed)
    
    return;
  }

  new->data = data;

  new->next = list->head;
  list->head = new;

  list->count++;
}

int sensor_filter_status(const sensor_list_t *list, sensor_status_t status, sensor_t *sensors)
{
  if (list == NULL || sensors == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] sensor_filter_status : NULL arguments)
    return 0;
  }

  sensor_node_t *node = list->head;
  int i = 0;

  while (node != NULL && i < list->count)
  {
    if (node->data.status == status)
    {
      sensors[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}


int sensor_filter_by_code(const sensor_list_t *list, const char *code, sensor_t *sensors){
  if (list == NULL || code == NULL || sensors == NULL)
  {
    // TODO: Implement a log system (ex.: (datatime) [ERROR] sensor_filter_by_code : NULL arguments)
    return 0;
  }

  sensor_node_t *node = list->head;
  int i = 0;

  while (node != NULL && i < list->count)
  {
    if (strcmp(node->data.code, code) == 0)
    {
      sensors[i] = node->data;
      i++;
    }

    node = node->next;
  }

  return i;
}

bool sensor_validate(sensor_t sensor)
{
  if (strlen(sensor.code) <= 4) return false;
  if (strlen(sensor.type) <= 6) return false;
  if (sensor.value < -100 || sensor.value > 10000) return false;
  if (strlen(sensor.unit) < 1) return false;
  if (sensor.status < SENSOR_OK || sensor.status > SENSOR_NET_FAILURE) return false;
  else return true;
}

sensor_t sensor_create_from_line(char *line)
{
  sensor_t sensor;
  char *saveptr; // thread safe instead of a static pointer by strtok
  char *token = strtok_r(line, ";", &saveptr); // ';' as delimiter
  
  int column = 0;

  while (token != NULL)
  {
    ++column;
    
    if (column == 1) sensor.read_at = sensor_format_timestamp(token);
    if (column == 5) snprintf(sensor.code, CODE_MAX, "%s", token);
    else if (column == 6) snprintf(sensor.type, STRING_MAX, "%s", token);
    else if (column == 8) sscanf(token, "%f", &sensor.value);
    else if (column == 9) snprintf(sensor.unit, UNIT_MAX, "%s", token);
    else if (column == 10) sensor.status = sensor_string_to_status(token);

    token = strtok_r(NULL, ";", &saveptr);
  }

  return sensor;
}

void sensor_insert_from_file(sensor_list_t *list, const char *filepath)
{
  FILE *file = fopen(filepath, "r");
  if (file == NULL) return;

  char buffer[250]; // store until '\n'
  
  // ignore until found 'timestamp;sequence;site;rack...'
  while (fgets(buffer, sizeof(buffer), file))
  {
    if (buffer[0] != '#') break;  
  }
  
  while (fgets(buffer, sizeof(buffer), file))
  {
    sensor_t sensor = sensor_create_from_line(buffer);

    if (sensor_validate(sensor) == true)
    {
      char buffer[DATETIME_MAX];
      get_datetime(sensor.read_at, buffer);
      printf("CODE: %s | TYPE: %s | VALUE: %.2f | UNIT: %s | STATUS: %s | READ AT: %s\n\n", sensor.code, sensor.type, sensor.value, sensor.unit, sensor_status_to_string(sensor.status), buffer);
      sensor_list_insert(list, sensor);
    }
  }

  fclose(file);
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
