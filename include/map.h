#pragma once

#include "alloc.h"
#include <stdbool.h>

#define Hashmap(key_ty, val_ty)                                                                                                            \
  {                                                                                                                                        \
    typeof(key_ty) k;                                                                                                                      \
    typeof(val_ty) v;                                                                                                                      \
  }                                                                                                                                        \
  struct _internal_hashmap

typedef bool (*EqFunc)(const void *a, const void *b);

typedef int (*HashFunc)(const void *a);

struct _internal_hashmap_header {
  Allocator *allocator;
  size_t capacity;
  size_t key_type_size;
  size_t value_type_size;
  HashFunc key_hash_func;
  EqFunc key_eq_func;
  EqFunc val_eq_func;
};

struct _internal_hashmap {
  void *keys;
  void *values;
  bool *present_keys;
  size_t len;
  struct _internal_hashmap_header header;
};

struct _internal_hashmap _internal_hashmap_new(struct _internal_hashmap_header h);

void *_internal_hashmap_value(struct _internal_hashmap *hashmap, const void *key);

void *_internal_hashmap_key(struct _internal_hashmap *hashmap, const void *val);

#define MAP_DEFAULT_CAPACITY 200

#define hashmap_new(key_ty, val_ty, _allocator, _key_hash_func, _key_eq_func, _val_eq_func)                                                \
  _internal_hashmap_new((struct _internal_hashmap_header){.allocator = _allocator,                                                         \
                                                          .capacity = MAP_DEFAULT_CAPACITY,                                                \
                                                          .key_type_size = sizeof(key_ty),                                                 \
                                                          .value_type_size = sizeof(val_ty),                                               \
                                                          .key_hash_func = _key_hash_func,                                                 \
                                                          .key_eq_func = _key_eq_func,                                                     \
                                                          .val_eq_func = _val_eq_func})

#define hashmap_new_capacity(key_ty, val_ty, _allocator, _key_hash_func, _key_eq_func, _val_eq_func, _capacity)                            \
  _internal_hashmap_new((struct _internal_hashmap_header){.allocator = _allocator,                                                         \
                                                          .capacity = _capacity,                                                           \
                                                          .key_type_size = sizeof(key_ty),                                                 \
                                                          .value_type_size = sizeof(val_ty),                                               \
                                                          .key_hash_func = _key_hash_func,                                                 \
                                                          .key_eq_func = _key_eq_func,                                                     \
                                                          .val_eq_func = _val_eq_func})

#define hashmap_value(map, key, ...) __VA_OPT__(*(__VA_ARGS__ *)) _internal_hashmap_value(map, key)

#define hashmap_key(map, val, ...) __VA_OPT__(*(__VA_ARGS__ *)) _internal_hashmap_key(map, key)

bool hashmap_insert(struct _internal_hashmap *hashmap, void *key, void *val);

void hashmap_free(struct _internal_hashmap *hashmap);
