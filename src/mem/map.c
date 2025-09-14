#include "../../include/map.h"
#include "../../include/shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _internal_hashmap _internal_hashmap_new(struct _internal_hashmap_header h) {
  struct _internal_hashmap map = (struct _internal_hashmap){
      .keys = h.allocator->alloc(h.capacity * h.key_type_size),
      .present_keys = h.allocator->alloc(h.capacity * sizeof(bool)),
      .values = h.allocator->alloc(h.capacity * h.value_type_size),
      .header = h,
  };
  for (size_t i = 0; i < h.capacity; i++) {
    map.present_keys[i] = false;
  }
  return map;
}

static void _internal_hashmap_memcpy(struct _internal_hashmap *hashmap, size_t index, const void *key, const void *val) {
  memcpy(hashmap->keys + index * hashmap->header.key_type_size, key, hashmap->header.key_type_size);
  memcpy(hashmap->values + index * hashmap->header.value_type_size, val, hashmap->header.value_type_size);
}

bool hashmap_insert(struct _internal_hashmap *hashmap, void *key, void *val) {
  // size_t len = hashmap->len;
  // size_t prev_capacity = hashmap->header.capacity;
  // size_t new_capacity = prev_capacity * 2;
  // Allocator *allocator = hashmap->header.allocator;
  // size_t new_key_size = hashmap->header.key_type_size * new_capacity;
  // size_t new_value_size = hashmap->header.value_type_size * new_capacity;
  // if (len >= prev_capacity) {
  //   void *temp_keys = allocator->alloc(new_key_size);
  //   void *temp_values = allocator->alloc(new_value_size);
  //
  //  memcpy(temp_keys, hashmap->keys, len * hashmap->header.key_type_size);
  //  memcpy(temp_values, hashmap->values, len * hashmap->header.value_type_size);
  //  allocator->dealloc(hashmap->keys);
  //  allocator->dealloc(hashmap->values);
  //  hashmap->keys = temp_keys;
  //  hashmap->values = temp_values;
  //}
  //
  // memcpy((char *)hashmap->keys + len * hashmap->header.key_type_size, key, hashmap->header.key_type_size);
  // memcpy((char *)hashmap->values + len * hashmap->header.value_type_size, val, hashmap->header.value_type_size);
  // hashmap->len++;

  int hash = hashmap->header.key_hash_func(key);
  size_t index = hash % hashmap->header.capacity;
  printf("Index: %zu\n", index);
  if (!hashmap->present_keys[index]) {
    _internal_hashmap_memcpy(hashmap, index, key, val);
    hashmap->present_keys[index] = true;
    return true;
  } else {
    void *key_at_index = hashmap->keys + index * hashmap->header.key_type_size;
    if (hashmap->header.key_eq_func(key, key_at_index)) {
      return false;
    } else {
      for (size_t i = index; i < hashmap->header.capacity; i++) {
        if (!hashmap->present_keys[i]) {
          printf("Inserted at index: %zu\n", i);
          _internal_hashmap_memcpy(hashmap, i, key, val);
          hashmap->present_keys[i] = true;
          return true;
        } else {
          void *collision_key = hashmap->keys + i * hashmap->header.key_type_size;
          if (hashmap->header.key_eq_func(key, collision_key)) {
            return false;
          }
        }
      }
#ifdef DEBUG_BUILD
      printf("Reached end of map\n");
#endif
      return false;
    }
  }
  return false;
}

void *_internal_hashmap_value(struct _internal_hashmap *hashmap, const void *key) {
  int hash = hashmap->header.key_hash_func(key);
  size_t index = hash % hashmap->header.capacity;
  if (hashmap->present_keys[index]) {
    void *key_at_index = hashmap->keys + index * hashmap->header.key_type_size;
    if (hashmap->header.key_eq_func(key, key_at_index)) {
      return hashmap->values + index * hashmap->header.value_type_size;
    } else {
      for (size_t i = index; i < hashmap->header.capacity; i++) {
        if (hashmap->present_keys[i]) {
          void *key_at_index_col = hashmap->keys + i * hashmap->header.key_type_size;
          if (hashmap->header.key_eq_func(key, key_at_index_col)) {
            return hashmap->values + i * hashmap->header.value_type_size;
          }
        } else {
          return NULL;
        }
      }
    }
  }
  return NULL;
}

void hashmap_free(struct _internal_hashmap *hashmap) {
  free(hashmap->keys);
  free(hashmap->values);
}
