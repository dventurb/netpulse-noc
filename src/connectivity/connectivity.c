#include "connectivity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ping_result_t *connectivity_run_ping(const char *ip_address, int count, int timeout, int packet_size)
{
  ping_result_t *result = malloc(sizeof(ping_result_t));
  if (result == NULL) return NULL;

  char input[200];
  FILE *file = NULL;

  #ifdef WIN32
    snprintf(input, sizeof(input), "ping -n %d -w %d -l %d %s", count, timeout * 1000, packet_size, ip_address);
  #endif
  #ifdef LINUX
    snprintf(input, sizeof(input), "ping -c %d -W %d -s %d %s", count, timeout, packet_size, ip_address);
  #endif
  #ifdef OSX
    snprintf(input, sizeof(input), "ping -c %d -t %d -s %d %s", count, timeout, packet_size, ip_address);
  #endif

  file = popen(input, "r");
  if (file == NULL) return NULL;

  size_t bytes_read = fread(result->output, 1, sizeof(result->output) - 1, file);
  result->output[bytes_read] = '\0';

  pclose(file);

  return result;
}

bool connectivity_check_ping(const char *string)
{
  char check[50];

  #ifdef WIN32 
    snprintf(check, sizeof(check), "100%% loss");
  #else
    snprintf(check, sizeof(check), "100%% packet loss");
  #endif

  if (strstr(string, check) != NULL)
  {
    return false;
  }

  return true;
}
