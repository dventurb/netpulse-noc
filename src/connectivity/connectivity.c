#include "connectivity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

ping_result_t *connectivity_run_ping(const char *ip_address, int count, int timeout, int packet_size)
{
  ping_result_t *result = calloc(1, sizeof(ping_result_t));
  if (result == NULL) return NULL;

  FILE *file = NULL;

  #ifdef WIN32
    snprintf(result->input, sizeof(result->input), "ping -n %d -w %d -l %d %s\n", count, timeout * 1000, packet_size, ip_address);
  #endif

  #ifdef LINUX
    snprintf(result->input, sizeof(result->input), "ping -c %d -W %d -s %d %s\n", count, timeout, packet_size, ip_address);
  #endif

  #ifdef OSX
    snprintf(result->input, sizeof(result->input), "ping -c %d -t %d -s %d %s\n", count, timeout, packet_size, ip_address);
  #endif

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  file = popen(result->input, "r");
  if (file == NULL) return NULL;

  size_t bytes_read = fread(result->output, 1, sizeof(result->output) - 1, file);
  result->output[bytes_read] = '\0';

  pclose(file);

  clock_gettime(CLOCK_MONOTONIC, &end);
  result->duration = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;

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

void connectivity_get_ping_latency(ping_result_t *result)
{
  if (result->responded == false)
  {
    result->min_latency = 0.0;
    result->avg_latency = 0.0;
    result->max_latency = 0.0;
    return;
  }

  #ifdef WIN32
    char *line = strstr(result->output, "Average =");
    if (line != NULL)
      sscanf(line, "Average = %fms", &result->avg_latency);

  #else
    char *line = strstr(result->output, "rtt min/avg/max");
    if (line != NULL)
    {
      char *start_iter = strchr(line, '=');
      start_iter++;

      char *old = setlocale(LC_NUMERIC, NULL);
      setlocale(LC_NUMERIC, "C");

      sscanf(start_iter, " %f/%f/%f", &result->min_latency, &result->avg_latency, &result->max_latency);

    setlocale(LC_NUMERIC, old);
    }
  #endif
}

void connectivity_get_ping_packets_stats(ping_result_t *result)
{
  #ifdef WIN32
    int lost = 0;

    char *line = strstr(result->output, "Packets: Sent =");
    if (line != NULL)
      sscanf(line, "Packets: Sent = %d, Received = %d, Lost = %d", &result->packets_sent, &result->packets_received, &lost);
    result->packets_loss_percent = (lost * 100) / result->packets_sent;

  #else
    char *line = strstr(result->output, "ping statistics ---");
    if (line != NULL)
    {
      char *new_line = strchr(line, '\n');
      new_line++;
      sscanf(new_line, "%d packets transmitted, %d received, %d%% packet loss", &result->packets_sent, &result->packets_received, &result->packets_loss_percent);
    }
  #endif
}
