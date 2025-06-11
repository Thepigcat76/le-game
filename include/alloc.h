#pragma once

#include "bump.h"
#include <stdlib.h>

typedef struct {
  void *(*alloc)(size_t bytes);
  void (*dealloc)(void *ptr);
} Allocator;

extern Allocator HEAP_ALLOCATOR;
extern Allocator GLOBAL_BUMP_ALLOCATOR;

extern Bump GLOBAL_BUMP;

void alloc_init();
