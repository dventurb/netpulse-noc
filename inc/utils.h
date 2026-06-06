#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stdbool.h>

typedef enum {
  SEARCH_ID,
  SEARCH_IP,
  SEARCH_MAC,
  SEARCH_INVALID
} search_type_t;

void get_datetime(time_t time, char *string);
bool validate_ip_address(const char *ip_address);
bool validate_mac_address(const char *mac_address);
bool validate_ping_count(int count);
bool validate_ping_timeout(int timeout);
bool validate_ping_packet_size(int packet_size);
search_type_t detect_search_type(const char *text);
void convert_to_uppercase(const char *text, char *buffer);

#endif
