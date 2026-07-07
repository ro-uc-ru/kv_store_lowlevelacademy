# include "kv.h"
# include <string.h>
# include <stdlib.h>
# define TOMBSTONE 0x1

// fn kv_init
// params:
// - capacity: total capacity for the key value store
// return types:
// - pointer to a kv store with capacity the same as the param: if there is no error 
// - NULL: if there is any error or capacity is 0
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
// -  0: success 
// - -1: on error
// - -2: not found
int kv_put(kv_t* db, char* key, char* value) {
  if (!db || !key || !value) return -1; //check if any of the params is null, exit 

  size_t idx = hash(key, db -> capacity);
  for (size_t i = 0; i < db -> capacity; i++) {
    size_t real_idx = (idx + i) % db -> capacity;
    kv_entry_t* entry = &db -> entries[real_idx];

    if (entry -> key && entry -> key != (void*) TOMBSTONE && !strcmp(entry -> key, key) ) {
      char* newval = strdup(value);
      if (!newval) {
        free(newval);
        return -1;
      }
      free(entry -> value);
      entry -> value = newval;
      return 0;
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
      return 0;
    }
  }
  return -2;
}


// fn kv_get
// params:
// - db: pointer to a key value store (db)
// - key: pointer to the key value
// return types:
// - pointer to the key: No error
// - NULL: if not found
char* kv_get(kv_t* db, char* key) {
  if (!db || !key) return NULL;
  size_t idx = hash(key, db -> capacity);

  for (size_t i = 0; i < db -> capacity; i++) {
    size_t real_idx = (idx + i) % db -> capacity;
    kv_entry_t* entry = &db -> entries[real_idx];

    // there is no key, therefore no value
    if (entry -> key == NULL ) {
      return NULL;
    }

    // find an entry and the keys match
    if (entry -> key && 
      entry -> key != (void*) TOMBSTONE && 
      !strcmp(entry ->key, key)) {
        return entry -> value;
      }
  }

  return NULL;
}

// fn kv_delete
// params:
// - db: pointer to a key value store (db)
// - key: pointer to the key value
// return types:
// -  idx of deletion: on succes deletion
// - -1: if there is an error
int kv_delete(kv_t* db, char* key) {
   if (!db || !key) return -1;
  size_t idx = hash(key, db -> capacity);

  for (size_t i = 0; i < db -> capacity ; i++) {
    size_t real_idx = (idx + i) % db -> capacity;
    kv_entry_t* entry = &db -> entries[real_idx];

    // we hit an empty entry
    if (entry -> key == NULL){
      return -1;
    }

    if (entry -> key &&
      entry -> key != (void*) TOMBSTONE &&
      !strcmp(entry -> key, key)
    ) {
      free(entry -> key);
      free(entry -> value);
      db -> count--;
      entry -> key = (void*) TOMBSTONE;
      entry -> value = NULL;
      return real_idx;
    }

  }

  return -1;
}

// fn kv_free
// params:
// - db: pointer to a key value store (db)
// return types:
// -  0: on succes free of db
// - -1: if there is an error
void kv_free(kv_t* db) {
  if (!db) return;

  // free entries
  for (size_t i = 0;  i < db -> capacity; i++) {
    kv_entry_t *entry = &db -> entries[i];

    if (entry -> key && entry -> key != (void*) TOMBSTONE)  {
      free(entry -> key);
      free(entry -> value);
      entry -> key = NULL;
      entry -> value = NULL;
      db -> count--;
    }
  }

  free(db -> entries);
  free(db);
  return;
}


