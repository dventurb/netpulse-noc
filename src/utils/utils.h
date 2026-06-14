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


time_t parse_date_to_timestamp(const char *string);

void format_timestamp_to_datetime(time_t timestamp, char *buffer);
void format_current_date(char *buffer);

time_t get_day_start_timestamp(time_t timestamp);
time_t get_day_end_timestamp(time_t timestamp);

bool validate_ip_address(const char *ip_address);
bool validate_mac_address(const char *mac_address);

bool validate_ping_count(int count);
bool validate_ping_timeout(int timeout);
bool validate_ping_packet_size(int packet_size);

bool validate_equipment_id(const char *string);
bool validate_incident_number(const char *string);
bool validate_sensor_code(const char *string);
bool validate_date(const char *string);

search_type_t detect_search_type(const char *string);

void convert_to_uppercase(const char *string, char *buffer);

#endif
