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
search_type_t detect_search_type(const char *text);

#endif
