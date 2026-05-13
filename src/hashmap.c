#include "hashmap.h"

#include <ctype.h>
#include <string.h>

void hashmap_init(hashmap_t *hashmap)
{
  for (int i = 0; i < HASHMAP_SIZE; i++)
  {
    hashmap->buckets[i] = NULL;
  }

  hashmap->count = 0;
}

void hashmap_destroy(hashmap_t *hashmap_t)
{
  for (int i = 0; i < HASHMAP_SIZE; i++) 
  {
    hashmap_bucket_t *bucket = hashmap->buckets[i];

    while (bucket != NULL)
    {
      hashmap_bucket_t *next = bucket->next;

      free(bucket->key);
      free(bucket);

      bucket = next;
    }

    hashmap->bucket[i] = NULL;
  }

  hashmap->count = 0;
}

// djb2 hash function from: http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash(const char *key)
{
  unsigned long hash = 5381;
  int c;

  while (c = *key++)
  {
    hash = ((hash << 5) + hash) + tolower(c);
  }
  return hash % HASHMAP_SIZE;
}

void hashmap_insert(hashmap_t *hashmap, const char *key, void *reference)
{
  hashmap_bucket_t *new = malloc(sizeof(hashmap_bucket_t));
  if (new == NULL)
  {
    // TODO: Implement a log system (ex.: (datetime) [ERROR] hashmap_insert : malloc failed)
    return;
  }

  new->key = strdup(key);
  new->reference = reference;
  new->next = NULL;

  unsigned long h = hash(key);

  if (hashmap->buckets[h] != NULL) new->next = hashmap->buckets[h];

  hashmap->buckets[h] = new;
  hashmap->count++;
}

void hashmap_remove(hashmap_t *hashmap, const char *key)
{
  if (hashmap == NULL || key == NULL) return;

  unsigned long h = hash(key);
  
  hashmap_bucket_t *bucket_current = hashmap->buckets[h];
  hashmap_bucket_t *bucket_previous = NULL;

  while (bucket_current != NULL) 
  {
    if (strcmp(bucket_current->key, key) == 0)
    {
      if (bucket_previous != NULL) bucket_previous->next = bucket_current->next;

      else hashmap->buckets[h] = bucket_current->next;

      free(bucket_current.key);
      free(bucket_current);

      hashmap->count--;
      
      return;
    }

    bucket_previous = bucket_current;
    bucket_current = bucket_current->next;
  }
}

void *hashmap_get(hashmap_t *hashmap, const char *key)
{
  if (hashmap == NULL || key == NULL) return NULL;

  unsigned long h = hash(key);

  hashmap_bucket_t *bucket = hashmap->buckets[h];

  while (bucket != NULL)
  {
    if (strcmp(bucket->key, key) == 0)
    {
      return bucket->reference;
    }

    bucket = bucket->next;
  }

  return NULL;
}
