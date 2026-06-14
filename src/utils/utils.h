#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stdbool.h>
#include <glib.h> // gboolean and gpointer types

typedef gboolean (*callback_task)(gpointer data); // function pointer

typedef enum {
  SEARCH_EQUIPMENT_ID,
  SEARCH_INCIDENT_ID,
  SEARCH_SENSOR_CODE,
  SEARCH_IP,
  SEARCH_MAC,
  SEARCH_INVALID
} search_type_t;

time_t set_datetime(const char *date);
void get_datetime(time_t time, char *string);
void get_current_date(char *string);
time_t get_datetime_start(time_t date);
time_t get_datetime_end(time_t date);

bool validate_ip_address(const char *ip_address);
bool validate_mac_address(const char *mac_address);

bool validate_ping_count(int count);
bool validate_ping_timeout(int timeout);
bool validate_ping_packet_size(int packet_size);

bool validate_equipment_id(const char *text);
bool validate_incident_number(const char *text);
bool validate_sensor_code(const char *text);
bool validate_date(const char *text);

search_type_t detect_search_type(const char *text);

void convert_to_uppercase(const char *text, char *buffer);

#endif
