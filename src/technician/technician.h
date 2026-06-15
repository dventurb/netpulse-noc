#ifndef TECHNICIAN_H
#define TECHNICIAN_H

#include "macros.h"
#include "hashmap.h"
#include <stdbool.h>

typedef enum {
  ROLE_ADMIN,
  ROLE_TECHNICIAN
} technician_role_t;

typedef struct {
  int id; // TODO: Maybe change the id to a char (ex.: TE-xxx)
  char name[STRING_MAX];
  char username[USERNAME_MAX];
  char password_hash[PASSWORD_HASH_MAX];
  char avatar_path[STRING_MAX];
  technician_role_t role;
  bool is_active;
} technician_t;

typedef struct technician_node_t {
  technician_t data;
  struct technician_node_t *next;
  struct technician_node_t *previous;
} technician_node_t;

typedef struct {
  technician_node_t *head;
  int count;
  int next_id; // TODO: Maybe change the id to a char (ex.: TE-xxx)
} technician_list_t;


void technician_list_init(technician_list_t *list);
void technician_list_destroy(technician_list_t *list);

technician_node_t *technician_list_insert(technician_list_t *list, technician_t data);
void technician_list_remove(technician_list_t *list, technician_node_t *node);
technician_node_t *technician_list_reinsert(technician_list_t *list, technician_t data);

bool technician_exists_by_username(hashmap_t *username_index, const char *username);

int technician_filter_by_role(technician_list_t *list, technician_role_t role, technician_t *technicians);

#endif
