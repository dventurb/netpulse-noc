#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include "macros.h"
#include <time.h>

typedef enum {
  TYPE_ROUTER,
  TYPE_FIREWALL,
  TYPE_SWITCH,
  TYPE_ACCESS_POINT,
  TYPE_SERVER,
  TYPE_NAS,
  TYPE_UPS,
  TYPE_IP_CAMERA,
  TYPE_PRINTER,
  TYPE_OTHER
} equipment_type_t;

typedef enum {
  STATUS_FAILED,
  STATUS_MAINTENANCE,
  STATUS_OPERATIONAL,
  STATUS_DISABLED
} equipment_status_t;

typedef struct {
  int id; // TODO: Maybe change the id to a char (ex.: EQ-xxx)
  char name[STRING_MAX];
  equipment_type_t type;
  char vendor[STRING_MAX];
  char model[STRING_MAX];
  char ip_address[IP_MAX];
  char mac_address[MAC_MAX];
  char location[STRING_MAX];
  equipment_status_t status;
  time_t last_check;
} equipment_t;

typedef struct equipment_node_t {
  equipment_t data;
  struct equipment_node_t *next;
  struct equipment_node_t *previous;
} equipment_node_t;

typedef struct {
  equipment_node_t *head;
  equipment_node_t *tail;
  int count;
  int next_id; // TODO: Maybe change the id to a char (ex.: EQ-xxx)
} equipment_list_t;


typedef int (*callback_fn)(const equipment_node_t *, const equipment_node_t *); // Function Pointer

void equipment_list_init(equipment_list_t *list);
void equipment_list_destroy(equipment_list_t *list);
equipment_node_t *equipment_list_insert(equipment_list_t *list, equipment_t data);
void equipment_list_remove(equipment_list_t *list, equipment_node_t *node); // Flow: hashmap_get() -> return void * -> casting to equipment_node_t -> equipment_list_remove() 
equipment_node_t *equipment_list_reinsert(equipment_list_t *list, equipment_t data);
equipment_t *equipment_list_in_range(equipment_list_t *list, int start, int end, int *count);
void equipment_list_clone(equipment_list_t *source, equipment_list_t *destination);

void equipment_list_sort_by_status(equipment_list_t *list);
void equipment_list_sort_by_location(equipment_list_t *list);
void equipment_list_sort_by_type(equipment_list_t *list);

void equipment_update(equipment_t *equipment, equipment_t data);
void equipment_update_status(equipment_t *equipment, equipment_status_t status);
void equipment_update_last_check(equipment_t *equipment);

void equipment_filter(const equipment_list_t *list, equipment_status_t status, equipment_type_t type, equipment_list_t *filtered);
void equipment_filter_by_status(const equipment_list_t *list, equipment_status_t status, equipment_list_t *filtered);
void equipment_filter_by_type(const equipment_list_t *list, equipment_type_t type, equipment_list_t *filtered);

const char *equipment_status_to_string(equipment_status_t status);
const char *equipment_type_to_string(equipment_type_t type);

int equipment_get_count(equipment_list_t *list);
int equipment_get_number_status(equipment_list_t *list, equipment_status_t status);
void equipment_format_id(int id, char *buffer);

#endif
