# include "kv.h"
# include <string.h>
# include <stdlib.h>
# define TOMBSTONE 0x1

kv_t* kv_init(size_t capacity) {
  if (capacity == 0) return NULL;

  kv_t* table = malloc(sizeof(kv_t));
  if (table == NULL) {
    return NULL;
  }
  table -> capacity = capacity;
  table -> count = 0;
  table -> entries = calloc(sizeof(kv_entry_t), capacity);
  if (table -> entries == NULL) {
    free(table);
    table = NULL;
    return NULL;
  }


  return table;
}


size_t hash(char* value, int capacity) {
  size_t hash = 0x1337abcdffff4891;
  
  while (*value) {
    hash ^= *value;
    hash = hash << 8;
    hash += *value;
    value++;
  }
  // this hash method is not cryptographically useful, its just for learning purposes
  return hash % capacity; 
}

// fn kv_put
// params:
// - db: pointer to a key value store (db)
// - key: pointer to the key value
// - value: pointer to the value itself
// return types:
// - index of key: if no error
// - -1: on error
// - -2: not found
int kv_put(kv_t* db, char* key, char* value) {
  if (!db || !key || !value) return -1; //check if any of the params is null, exit 

  size_t idx = hash(key, db -> capacity);
  for (int i = 0; i < db -> capacity -1; i++) {
    size_t real_idx = (idx + i) % db -> capacity;
    kv_entry_t* entry = &db -> entries[real_idx];

    if (entry -> key && entry -> key != (void*) TOMBSTONE && !strcmp(entry -> key, key) ) {
      char* newval = strdup(value);
      if (!newval) {
        free(newval);
        return -1;
      }
      entry -> value = newval;
      return real_idx;
    }

    if (!entry -> key || entry -> key == (void*) TOMBSTONE) {
      char* newval = strdup(value);
      char* newkey = strdup(key);
      if (!newval || !newkey) {
        free(newkey);
        free(newval);
        return -1;
      }
      entry -> key = newkey;
      entry -> value = newval;
      db -> count++;
      return real_idx;
    }
  }
  return -2;
}