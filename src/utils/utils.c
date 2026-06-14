#include "utils.h"

#include "macros.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

time_t parse_date_to_timestamp(const char *string)
{
  struct tm tm = {0};
  sscanf(string, "%2d-%2d-%4d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year);

  tm.tm_year -= 1900; // years since 1900
  tm.tm_mon -= 1; // January is 0
  
  return mktime(&tm);
}

void format_timestamp_to_datetime(time_t timestamp, char *buffer)
{
  if (timestamp == 0) strcpy(buffer, "-");

  else 
  {
    struct tm *tm = localtime(&timestamp);
    strftime(buffer, DATETIME_MAX, "%d-%m-%Y %H:%M", tm);
  }
}

void format_current_date(char *buffer)
{
  time_t current_timestamp = time(NULL);

  struct tm *tm = localtime(&current_timestamp);
  strftime(buffer, DATE_MAX, "%d-%m-%Y", tm);
}

time_t get_day_start_timestamp(time_t timestamp)
{
  struct tm *tm_date = localtime(&timestamp);

  struct tm tm_start = *tm_date;
  tm_start.tm_hour = 0;
  tm_start.tm_min = 0;
  tm_start.tm_sec = 0;

  return mktime(&tm_start);
}

time_t get_day_end_timestamp(time_t timestamp)
{
  struct tm *tm_date = localtime(&timestamp);

  struct tm tm_end = *tm_date;
  tm_end.tm_hour = 23;
  tm_end.tm_min = 59;
  tm_end.tm_sec = 59;

  return mktime(&tm_end);
}

bool validate_ip_address(const char *ip_address)
{
  if (ip_address == NULL) return false;

  // 0.0.0.0 ... 255.255.255.255
  if (strlen(ip_address) < 7 || strlen(ip_address) > 15) return false;

  int digits[4];
  char garbage;

  int result = sscanf(ip_address, "%3d.%3d.%3d.%3d%c", &digits[0], &digits[1], &digits[2], &digits[3], &garbage);

  if (result != 4) return false;

  for (int i = 0; i < 4; i++) 
  {
    if (digits[i] < 0 || digits[i] > 255) return false;
  }

  return true;
}

bool validate_mac_address(const char *mac_address)
{
  if (mac_address == NULL) return false;

  // AA:BB:CC:DD:EE:FF
  if (strlen(mac_address) != 17) return false;

  char separator = ':'; // separator appears every 3 chars at positions (2, 5, 8, 11, 15).

  for (int i = 0; i < 17; i++) 
  {
    if ((i + 1) % 3 == 0)
    {
      if (mac_address[i] != separator) return false;
    }

    else 
    {
      if ((mac_address[i] < '0' || mac_address[i] > '9') && (mac_address[i] < 'A' || mac_address[i] > 'F') && (mac_address[i] < 'a' || mac_address[i] > 'f'))
        return false;
    }
  }

  return true;
}

bool validate_ping_count(int count)
{
  if (count >= 1 && count <= 20) return true;
  return false;
}

bool validate_ping_timeout(int timeout)
{
  if (timeout >= 1 && timeout <= 10) return true;
  return false;
}

bool validate_ping_packet_size(int packet_size)
{
  // 1500 bytes is the standard for MTU (1472 bytes + IP Header (20 bytes) + ICMP Header (8 bytes))
  if (packet_size >= 16 && packet_size <= 1472) return true; 
  return false;
}

bool validate_equipment_id(const char *string)
{
  int id;
  char garbage;

  if (sscanf(string, "EQ-%d%c", &id, &garbage) == 1) return true;
  else return false;
}

bool validate_incident_number(const char *string)
{
  int number;
  char garbage;

  if (sscanf(string, "IN-%d%c", &number, &garbage) == 1) return true;
  else return false;
}

bool validate_sensor_code(const char *string)
{
  const char *codes[] = {
    "TEMP_RACK",
    "HUM_RACK",
    "UPS_BAT",
    "UPS_ENERGIA",
    "FAN_RACK",
    "DOOR_RACK"
  };

  for (int i = 0; i < 5; i++) 
  {
    if (strncmp(codes[i], string, strlen(string)) == 0) return true;
  }

  return false;
}

bool validate_date(const char *string)
{
  int day, month, year;
  char garbage;

  if (sscanf(string, "%d-%d-%4d%c", &day, &month, &year, &garbage) != 3) return false;
  else return true;
}

// TODO: Add SEARCH_EQUIPMENT_ID, SEARCH_INCIDENT_ID, SEARCH_TECHNICIAN_ID
search_type_t detect_search_type(const char *string)
{
  if (string == NULL) return SEARCH_INVALID;
  
  if (validate_ip_address(string)) return SEARCH_IP;

  if (validate_mac_address(string)) return SEARCH_MAC;

  if (validate_equipment_id(string)) return SEARCH_EQUIPMENT_ID;

  if (validate_incident_number(string)) return SEARCH_INCIDENT_ID;

  if (validate_sensor_code(string)) return SEARCH_SENSOR_CODE;

  return SEARCH_INVALID;
}

void convert_to_uppercase(const char *string, char *buffer)
{
  int i;
  for (i = 0; string[i] != '\0'; i ++) 
  {
    if (string[i] >= 'a' && string[i] <= 'z')
      buffer[i] = string[i] - ('a' - 'A');
    else
       buffer[i] = string[i];
  }

  buffer[i] = '\0';
}
