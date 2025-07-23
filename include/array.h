#pragma once

#include "alloc.h"
#include "shared.h"
#include <string.h>

#define array_new(type, allocator) (type *)_internal_array_new(ARRAY_INITIAL_CAPACITY, sizeof(type), allocator)

#define array_new_capacity(type, capacity, allocator) (type *)_internal_array_new(capacity, sizeof(type), allocator)

#define array_len(arr) _internal_array_len(arr)

#define array_free(arr) _internal_array_free(arr)

#define array_foreach(arr, type, elem, ...)                                                                                                \
  do {                                                                                                                                     \
    size_t len = array_len(arr);                                                                                                           \
    type elem;                                                                                                                             \
    for (size_t _arr_foreach_index = 0; _arr_foreach_index < len; _arr_foreach_index++) {                                                         \
      elem = arr[_arr_foreach_index];                                                                                                      \
      __VA_ARGS__                                                                                                                          \
    }                                                                                                                                      \
  } while (0)

typedef struct {
  Allocator *allocator;
  size_t capacity;
  size_t len;
  size_t item_size;
} _InternalArrayHeader;

void *_internal_array_new(size_t capacity, size_t item_size, Allocator *allocator);

void _internal_array_set_len(void *arr, size_t len);

size_t _internal_array_len(void *arr);

void _internal_array_free(void *arr);

void _internal_array_add(void **arr_ptr, void *item, size_t item_size);

void _internal_array_remove(void *arr_ptr, size_t index);

void _internal_array_clear(void *arr_ptr);

#define array_add(arr, ...)                                                                                                                \
  do {                                                                                                                                     \
    __typeof__(*(arr)) _tmp = (__VA_ARGS__);                                                                                               \
    _internal_array_add((void **)&(arr), &_tmp, sizeof(_tmp));                                                                             \
  } while (0)

#define array_remove(array, index) _internal_array_remove(array, index)

#define array_clear(array) _internal_array_clear(array)
