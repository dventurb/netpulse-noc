#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "macros.h"

typedef enum {
  STATUS_OPERATIONAL,
  STATUS_FAILED,
  STATUS_MAINNTENANCE,
  STATUS_DISABLE
} equipment_status_t;

typedef struct {
  int id; // TODO: Maybe change the id to a char (ex.: EQ-xxx)
  char name[STRING_MAX];
  char type[STRING_MAX];
  char vendor[STRING_MAX];
  char model[STRING_MAX];
  char ip_address[IP_MAX];
  char mac_address[MAC_MAX];
  char location[STRING_MAX];
  equipment_status_t status
  // TODO: Struct time -> last_check
} equipment_t;

typedef struct equipment_node_t {
  equipment_t data;
  struct equipment_node_t *next;
  struct equipment_node_t *previous
} equipment_node_t;

typedef struct {
  equipment_node_t *head;
  equipment_node_t *tail;
  int count;
  int next_id // TODO: Maybe change the id to a char (ex.: EQ-xxx)
} equipment_list_t;

#endif
