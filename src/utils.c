#include "utils.h"

#include "macros.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

void get_datetime(time_t time, char *string)
{
  struct tm *tm = localtime(&time);
  strftime(string, DATETIME_MAX, "%d-%m-%Y %H:%M", tm);
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
