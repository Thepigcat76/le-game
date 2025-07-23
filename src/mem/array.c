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
    h->item_size = item_size;
    ptr = h + 1;
  }

  return ptr;
}

static void *_internal_array_double_size(void *arr, size_t item_size) {
  _InternalArrayHeader *h = ((_InternalArrayHeader *)arr) - 1;
  size_t arr_len = h->len;
  size_t arr_new_capacity = h->capacity * 2;
  Allocator *allocator = h->allocator;

  size_t new_size = item_size * arr_new_capacity + sizeof(_InternalArrayHeader);
  void *temp = allocator->alloc(new_size);
  if (!temp)
    return NULL;

  _InternalArrayHeader *new_h = (_InternalArrayHeader *)temp;
  *new_h = *h;  // Copy metadata
  new_h->capacity = arr_new_capacity;

  void *new_arr = new_h + 1;
  memcpy(new_arr, arr, arr_len * item_size);

  allocator->dealloc(h);
  return new_arr;
}

static void *_internal_array_prepare_add(void *arr, size_t item_size, size_t *out_len) {
  _InternalArrayHeader *h = ((_InternalArrayHeader *)(arr)) - 1;

  if (h->len >= h->capacity) {
    arr = _internal_array_double_size(arr, item_size);
    h = ((_InternalArrayHeader *)(arr)) - 1;
  }

  if (out_len) {
    *out_len = h->len;
  }

  return arr;
}

inline void _internal_array_add(void **arr_ptr, void *item, size_t item_size) {
  size_t len = 0;

  // Prepare and get updated pointer and target index
  void *arr = _internal_array_prepare_add(*arr_ptr, item_size, &len);
  *arr_ptr = arr;

  _InternalArrayHeader *h = ((_InternalArrayHeader *)arr) - 1;

  memcpy((char *)arr + len * item_size, item, item_size);
  h->len++;
}

void _internal_array_remove(void *arr_ptr, size_t index) {
  if (!arr_ptr)
    return;

  _InternalArrayHeader *h = ((_InternalArrayHeader *)arr_ptr) - 1;

  if (index >= h->len) {
#ifdef DEBUG_BUILD
#include <stdio.h>
    printf("Index %zu out of bounds for array of length %zu\n", index, h->len);
#endif
    return;
  }

  char *arr = (char *)arr_ptr;
  size_t item_size = h->item_size;

  char *dest = arr + index * item_size;
  char *src = arr + (index + 1) * item_size;
  size_t move_count = h->len - index - 1;

  if (move_count > 0) {
    memmove(dest, src, move_count * item_size);
  }

  h->len--;
}

void _internal_array_clear(void *arr_ptr) { _internal_array_set_len(arr_ptr, 0); }

void _internal_array_set_len(void *arr, size_t len) {
  _InternalArrayHeader *h = ((_InternalArrayHeader *)arr) - 1;
  h->len = len;
}

size_t _internal_array_len(void *arr) { return (((_InternalArrayHeader *)arr) - 1)->len; }

void _internal_array_free(void *arr) {
  _InternalArrayHeader *h = ((_InternalArrayHeader *)arr) - 1;
  h->allocator->dealloc(h);
}
