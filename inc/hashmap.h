#ifndef HASHMAP_H
#define HASHMAP_H

#include "macros.h"

typedef struct hash_bucket_t {
  char key[KEY_MAX];
  void *reference; // EQUIPMENT or TECHNICIAN
  struct hash_bucket_t *next; // Chaining (collision resolution)
} hash_bucket_t;

typedef struct {
  hash_bucket_t *buckets[HASHMAP_SIZE];
  int count;
} hashmap_t;

void hashmap_init(hashmap_t *hashmap);
void hashmap_destroy(hashmap_t *hashmap_t);
void hashmap_insert(hashmap_t *hashmap, const char *key, void *reference);
void hashmap_remove(hashmap_t *hashmap, const char *key);
void hashmap_get(hashmap_t *hashmap, const char *key);

#endif
