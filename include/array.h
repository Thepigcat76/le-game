#pragma once

#include "alloc.h"
#include "shared.h"
#include <string.h>

#define array_new(type, allocator) (type *)_internal_array_new(ARRAY_INITIAL_CAPACITY, sizeof(type), allocator)

#define array_new_capacity(type, capacity, allocator) (type *)_internal_array_new(capacity, sizeof(type), allocator)

#define array_len(arr) _internal_array_len(arr)

#define array_free(arr) _internal_array_free(arr)

typedef struct {
  Allocator *allocator;
  size_t capacity;
  size_t len;
} _InternalArrayHeader;

void *_internal_array_new(size_t capacity, size_t item_size, Allocator *allocator);

void _internal_array_set_len(void *arr, size_t len);

size_t _internal_array_len(void *arr);

void _internal_array_free(void *arr);

void _internal_array_add(void **arr_ptr, void *item, size_t item_size);

#define array_add(arr, value)                                                                                          \
  do {                                                                                                                 \
    __typeof__(*(arr)) _tmp = (value);                                                                                 \
    _internal_array_add((void **)&(arr), &_tmp, sizeof(_tmp));                                                          \
  } while (0)
