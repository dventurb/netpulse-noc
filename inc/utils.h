#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stdbool.h>

void get_datetime(time_t time, char *string);
bool validate_ip_address(const char *ip_address);
bool validate_mac_address(const char *mac_address);

#endif
