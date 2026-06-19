#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <stdbool.h>

typedef struct {
  bool responded;

  int packets_sent;
  int packets_received;
  int packets_loss_percent;

  float avg_latency;
  float min_latency;
  float max_latency;

  float duration;
  
  char input[100];
  char output[4800]; // maximum theoretical size is ~4318 (not sure about that)

} ping_result_t;

ping_result_t *connectivity_run_ping(const char *ip_address, int count, int timeout, int packet_size);
bool connectivity_check_ping(const char *string);
void connectivity_get_ping_packets_stats(ping_result_t *result);
void connectivity_get_ping_latency(ping_result_t *result);

void connectivity_generate_log(const char *ip_address, float latency, bool responded);

#endif
