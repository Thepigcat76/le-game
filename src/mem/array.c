#include "../../include/array.h"
#include <string.h>

void *_internal_array_new(size_t capacity, size_t item_size, Allocator *allocator) {
  void *ptr = NULL;
  size_t size = item_size * capacity + sizeof(_InternalArrayHeader);
  _InternalArrayHeader *h = allocator->alloc(size);

  if (h) {
    h->capacity = capacity;
    h->len = 0;
    h->allocator = allocator;
    ptr = h + 1;
  }

  return ptr;
}

static void *_internal_array_double_size(void *arr, size_t item_size) {
  _InternalArrayHeader *h = ((_InternalArrayHeader *)arr) - 1;
  size_t arr_len = h->len;
  size_t arr_new_capacity = h->capacity * 2;
  Allocator *arr_allocator = h->allocator;
  size_t size = item_size * arr_new_capacity + sizeof(_InternalArrayHeader);
  void *temp = h->allocator->alloc(size);
  memcpy(temp, h, sizeof(_InternalArrayHeader) + h->len * item_size);
  h->allocator->dealloc(h);
  return ((_InternalArrayHeader *)temp) + 1;
}

static size_t _internal_array_prepare_add(void *arr, size_t item_size) {
  _InternalArrayHeader *h = ((_InternalArrayHeader *)(arr)) - 1;
  if (h->len >= h->capacity) {
    arr = _internal_array_double_size(arr, item_size);
    h = ((_InternalArrayHeader *)(arr)) - 1;
  }
  return h->len;
}

inline void _internal_array_add(void **arr_ptr, void *item, size_t item_size) {
    void *arr = *arr_ptr;
    size_t len = _internal_array_prepare_add(arr, item_size);

    _InternalArrayHeader *h = ((_InternalArrayHeader *)arr) - 1;
    memcpy((char *)arr + len * item_size, item, item_size);
    _internal_array_set_len(arr, len + 1);

    // In case realloc happened
    *arr_ptr = arr;
}

void _internal_array_set_len(void *arr, size_t len) {
  _InternalArrayHeader *h = ((_InternalArrayHeader *)arr) - 1;
  h->len = len;
}

size_t _internal_array_len(void *arr) { return (((_InternalArrayHeader *)arr) - 1)->len; }

void _internal_array_free(void *arr) {
  _InternalArrayHeader *h = ((_InternalArrayHeader *)arr) - 1;
  h->allocator->dealloc(h);
}
