#include <stdio.h>

#define HASHTABLE_IMPLEMENTATION
#include "hashtable.h"

int main(void) {
  printf("Hello World\n");

  Hash_Table table = {0};
  ht_alloc(&table, 0, NULL, NULL);

  ht_insert(&table, "Person 1", "Paul");
  ht_insert(&table, "Person 2", "Jean");

  printf("Person 1: %s\n", (char *)ht_get(&table, "Person 1"));
  printf("Person 2: %s\n", (char *)ht_get(&table, "Person 2"));

  ht_remove(&table, "Person 1");

  printf("Person 1: %s\n", (char *)ht_get(&table, "Person 1"));

  char *p2_name = NULL;
  if (ht_find(&table, "Person 2", &p2_name) < 0) {
    printf("Did not find Person 2\n");
  }
  printf("Person 2: %s\n", p2_name);

  Hash_Table ids = {0};
  ht_alloc(&ids, 0, NULL, NULL);

  int k1 = 1;
  int k2 = 2;
  int v1 = 2;
  int v2 = 4;

  ht_insert(&ids, &k1, &v1);
  ht_insert(&ids, &k2, &v2);

  int *r1 = NULL;
  ht_find(&ids, &k1, &r1);

  printf("The value of k1 is %d\n", *r1);

  return 0;
}