#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_MAX_CAPACITY (1024)

typedef struct Hash_Table_s Hash_Table;
typedef struct Hash_Table_El_s Hash_Table_El;

struct Hash_Table_s {
  Hash_Table_El **elements;
  size_t *capacity;
  size_t *size;

  size_t max_capacity;

  size_t (*hash)(const void *);
  int (*compare)(const void *, const void *);
};

struct Hash_Table_El_s {
  void *key;
  void *value;
};

int ht_alloc(
  Hash_Table *table,
  size_t max_capacity,
  size_t (*hash)(const void *),
  int (*compare)(const void *, const void *)
);
int ht_free(Hash_Table *table);
int ht_clear(Hash_Table *table);

int ht_insert(Hash_Table *table, const void *key, const void *value);
int ht_insert_el(Hash_Table *table, Hash_Table_El *element);

int ht_remove(Hash_Table *table, const void *key);
int ht_remove_el(Hash_Table *table, Hash_Table_El *element);

int ht_find(Hash_Table *table, void *key, void *value);
int ht_find_el(Hash_Table *table, Hash_Table_El *element);

void *ht_get(Hash_Table *table, const void *key);
void *ht_get_el(Hash_Table *table, Hash_Table_El *element);

#endif  // HASHTABLE_H


#define HASHTABLE_IMPLEMENTATION
#ifdef HASHTABLE_IMPLEMENTATION

// Private functions forward declarations
size_t ht__hash(const void *key);
int ht__compare(const void *key_a, const void *key_b);
int ht__is_initialized(Hash_Table *table);

int ht_alloc(
  Hash_Table *table,
  size_t max_capacity,
  size_t (*hash)(const void *),
  int (*compare)(const void *, const void *)
) {
  if (table == NULL) {
    return -1;
  }

  if (table->elements != NULL ||
    table->capacity != NULL ||
    table->size != NULL
  ) {
    return -1;
  }

  table->max_capacity = HASH_TABLE_MAX_CAPACITY;
  table->hash = ht__hash;
  table->compare = ht__compare;

  if (max_capacity > 0) table->max_capacity = max_capacity;
  if (hash != NULL) table->hash = hash;
  if (compare != NULL) table->compare = compare;

  table->elements = calloc(table->max_capacity, sizeof(Hash_Table_El *));
  table->capacity = calloc(table->max_capacity, sizeof(size_t));
  table->size = calloc(table->max_capacity, sizeof(size_t));
  if (table->elements == NULL || table->capacity == NULL || table->size == NULL) {
    printf("[ERROR]: "
           "Error in ht_alloc: "
           "Could not allocate elements or capacity or size\n"
    );
    return -1;
  }

  for (size_t i = 0; i < table->max_capacity; i++) {
    table->elements[i] = calloc(1, sizeof(Hash_Table_El));
    if (table->elements[i] == NULL) {
      printf("[ERROR]: "
             "Error in ht_alloc: "
             "Could not allocate elements\n"
      );
      return -1;
    }

    table->capacity[i] = 1;
    table->size[i] = 0;
  }

  return 0;
}

int ht_free(Hash_Table *table) {
  if (table == NULL) {
    return -1;
  }

  for (size_t i = 0; i < table->max_capacity; i++) {
    free(table->elements[i]);
  }
  free(table->elements);
  free(table->capacity);
  free(table->size);

  table->elements = NULL;
  table->capacity = NULL;
  table->size = NULL;

  return 0;
}

int ht_clear(Hash_Table *table) {
  if (table == NULL) {
    return -1;
  }

  if (table->elements == NULL) {
    printf("[ERROR]: "
           "Error in ht_clear: "
           "HashTable not yet initialized\n"
    );
    return -1;
  }

  for (size_t i = 0; i < table->max_capacity; i++) {
    table->size[i] = 0;
  }

  return 0;
}


int ht_insert(Hash_Table *table, const void *key, const void *value) {
  if (table == NULL || key == NULL || value == NULL) {
    return -1;
  }

  if (ht__is_initialized(table) < 0) {
    return -1;
  }

  size_t idx = table->hash(key) % table->max_capacity;

  // Resize if needed
  if (table->size[idx] >= table->capacity[idx]) {
    Hash_Table_El *new = calloc(table->capacity[idx] * 2, sizeof(Hash_Table_El));
    if (new == NULL) {
      printf("[ERROR]: "
            "Error in ht_insert: "
            "Failed to allocate new HashTable element array\n"
      );
      return -1;
    }

    memcpy(new, table->elements[idx], table->size[idx] * sizeof(Hash_Table_El));
    free(table->elements[idx]);
    table->elements[idx] = new;
    table->capacity[idx] *= 2;
  }

  // Check if already present
  // If it is update with new value
  for (size_t i = 0; i < table->size[idx]; i++) {
    Hash_Table_El *curr = &table->elements[idx][i];
    if (table->compare(key, curr->key) == 0) {
      curr->value = (void *)value;
      return 0;
    }
  }

  table->elements[idx]->key = (void *)key;
  table->elements[idx]->value = (void *)value;
  table->size[idx]++;

  return 0;
}

int ht_insert_el(Hash_Table *table, Hash_Table_El *element) {
  if (table == NULL || element == NULL || element->key == NULL || element->value == NULL) {
    return -1;
  }

  if (ht__is_initialized(table) < 0) {
    return -1;
  }

  size_t idx = table->hash(element->key) % table->max_capacity;

  // Resize if needed
  if (table->size[idx] >= table->capacity[idx]) {
    Hash_Table_El *new = calloc(table->capacity[idx] * 2, sizeof(Hash_Table_El));
    if (new == NULL) {
      printf("[ERROR]: "
            "Error in ht_insert_el: "
            "Failed to allocate new HashTable element array\n"
      );
      return -1;
    }

    memcpy(new, table->elements[idx], table->size[idx] * sizeof(Hash_Table_El));
    free(table->elements[idx]);
    table->elements[idx] = new;
    table->capacity[idx] *= 2;
  }

  // Check if already present
  // If it is update with new value
  for (size_t i = 0; i < table->size[idx]; i++) {
    Hash_Table_El *curr = &table->elements[idx][i];
    if (table->compare(element->key, curr->key) == 0) {
      curr->value = element->value;
      return 0;
    }
  }

  table->elements[idx]->key = element->key;
  table->elements[idx]->value = element->value;
  table->size[idx]++;

  return 0;
}

int ht_remove(Hash_Table *table, const void *key) {
  if (table == NULL || key == NULL) {
    return -1;
  }

  if (ht__is_initialized(table) < 0) {
    return -1;
  }

  size_t idx = table->hash(key) % table->max_capacity;

  for (size_t i = 0; i < table->size[idx]; i++) {
    Hash_Table_El curr = table->elements[idx][i];
    if (table->compare(key, curr.key) == 0) {
      memcpy(table->elements[idx] + i, table->elements[idx] + i + 1, sizeof(Hash_Table_El) * (table->size[idx] - i));
      table->size[idx]--;

      return 0;
    }
  }

  return -1;
}

int ht_remove_el(Hash_Table *table, Hash_Table_El *element) {
  if (table == NULL || element == NULL || element->key == NULL) {
    return -1;
  }

  if (ht__is_initialized(table) < 0) {
    return -1;
  }

  size_t idx = table->hash(element->key) % table->max_capacity;

  for (size_t i = 0; i < table->size[idx]; i++) {
    Hash_Table_El curr = table->elements[idx][i];
    if (table->compare(element->key, curr.key) == 0) {
      memcpy(table->elements[idx] + i, table->elements[idx] + i + 1, sizeof(Hash_Table_El) * (table->size[idx] - i));
      table->size[idx]--;

      return 0;
    }
  }

  return -1;
}

int ht_find(Hash_Table *table, void *key, void *value_address) {
  if (table == NULL || key == NULL) {
    return -1;
  }

  if (ht__is_initialized(table) < 0) {
    return -1;
  }

  void **value = (void **)value_address;
  size_t idx = table->hash(key) % table->max_capacity;

  for (size_t i = 0; i < table->size[idx]; i++) {
    Hash_Table_El *curr = &table->elements[idx][i];
    if (table->compare(key, curr->key) == 0) {
      *value = curr->value;
      return 0;
    }
  }

  return -1;
}

int ht_find_el(Hash_Table *table, Hash_Table_El *element) {
  if (table == NULL || element == NULL || element->key == NULL) {
    return -1;
  }

  if (ht__is_initialized(table) < 0) {
    return -1;
  }

  size_t idx = table->hash(element->key) % table->max_capacity;

  for (size_t i = 0; i < table->size[idx]; i++) {
    Hash_Table_El *curr = &table->elements[idx][i];
    if (table->compare(element->key, curr->key) == 0) {
      element->value = curr->value;
      return 0;
    }
  }

  return -1;
}

void *ht_get(Hash_Table *table, const void *key) {
  if (table == NULL || key == NULL) {
    return NULL;
  }

  if (ht__is_initialized(table) < 0) {
    return NULL;
  }

  size_t idx = table->hash(key) % table->max_capacity;

  for (size_t i = 0; i < table->size[idx]; i++) {
    Hash_Table_El *curr = &table->elements[idx][i];
    if (table->compare(key, curr->key) == 0) {
      return curr->value;
    }
  }

  return NULL;
}

void *ht_get_el(Hash_Table *table, Hash_Table_El *element) {
  if (table == NULL || element == NULL || element->key == NULL) {
    return NULL;
  }

  if (ht__is_initialized(table) < 0) {
    return NULL;
  }

  size_t idx = table->hash(element->key) % table->max_capacity;

  for (size_t i = 0; i < table->size[idx]; i++) {
    Hash_Table_El *curr = &table->elements[idx][i];
    if (table->compare(element->key, curr->key) == 0) {
      return curr->value;
    }
  }

  return NULL;
}


size_t ht__hash(const void *key) {
  // One-byte-at-a-time hash based on Murmur's mix
  // Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp
  const char *str = (char *)key;
  size_t h = 1;

  for(; *str; str++) {
    h ^= *str;
    h *= 0x5bd1e995;
    h ^= h >> 15;
  }

  return h;
}

int ht__compare(const void *key_a, const void *key_b) {
  return strcmp(key_a, key_b);
}

int ht__is_initialized(Hash_Table *table) {
  if (table->elements == NULL ||
      table->capacity == NULL ||
      table->size == NULL ||
      table->hash == NULL ||
      table->compare == NULL)
  {
    printf("[ERROR]: "
           "Error in ht__is_initialized: "
           "HashTable not yet initialized\n"
    );
    return -1;
  }

  return 0;
}

#endif  // HASH_TABLE_IMPLEMENTATION