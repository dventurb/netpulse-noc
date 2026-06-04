#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include <stdbool.h>

typedef struct {
  bool responded;

  int packets_sent;
  int packets_received;
  
  char output[4800]; // maximum theoretical size is ~4318 (not sure about that)
} ping_result_t;

ping_result_t *connectivity_ping(const char *ip_address, int count, int timeout, int packet_size);

#endif
