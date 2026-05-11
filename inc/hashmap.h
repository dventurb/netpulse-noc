#ifndef HASHMAP_H
#define HASHMAP_H

#include "macros.h"

typedef struct hash_bucket_t {
  char key[KEY_MAX];
  void *reference; // EQUIPMENT or TECHNICIAN
  struct hash_bucket_t *next; // Chaining (collision resolution)
}

typedef struct {
  hash_bucket_t *buckets[HASHMAP_SIZE];
  int size;
  int count;
} hashmap_t;

#endif
