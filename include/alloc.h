#pragma once

#include "bump.h"
#include <stdlib.h>

typedef struct {
  void *(*alloc)(size_t bytes);
  void (*dealloc)(void *ptr);
} Allocator;

extern Allocator HEAP_ALLOCATOR;
extern Allocator GLOBAL_BUMP_ALLOCATOR;

#define BUMP_ALLOCATOR(name, bump_ptr)                                                                                 \
  static void *_internal_##name##_alloc(size_t bytes) { return bump_alloc(bump_ptr, bytes); };                                       \
  static void _internal_##name##_dealloc(void *ptr){};                                                                 \
  static Allocator name = {.alloc = _internal_##name##_alloc, .dealloc = _internal_##name##_dealloc };

extern Bump GLOBAL_BUMP;

void alloc_init();
