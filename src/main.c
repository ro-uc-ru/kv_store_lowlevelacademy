# include <stdio.h>
# include "kv.h"

int main() {
  kv_t* table = kv_init(16);

  printf("%p\n", table);
  printf("%ld\n", table -> capacity);

  kv_put(table, "hehe", "haha");
  kv_put(table, "hoho", "lele");

  for (int i = 0; i < table -> capacity; i++) {
    if (table -> entries[i].key) {
      printf("[%d] %s: %s\n", 
        i, 
        table -> entries[i].key, 
        table -> entries[i].value);
    }
  }

  char* val = kv_get(table, "hehe");
  printf("%s\n", val);
  kv_delete(table, "hehe");
  val = NULL;
  val = kv_get(table, "hehe");
  printf("%p\n", val);

  return 0;
}