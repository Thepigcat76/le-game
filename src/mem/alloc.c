#include "../../include/alloc.h"
#include "../../include/bump.h"
#include "../../include/shared.h"
#include <stdint.h>
#include <stdlib.h>

Allocator HEAP_ALLOCATOR;
Allocator GLOBAL_BUMP_ALLOCATOR;

static void *heap_allocator_alloc(size_t bytes) { return malloc(bytes); }

static void heap_allocator_dealloc(void *ptr) { free(ptr); }

Bump GLOBAL_BUMP;

static void *bump_allocator_alloc(size_t bytes) { return bump_alloc(&GLOBAL_BUMP, bytes); }

static void bump_allocator_dealloc(void *ptr) {}

void alloc_init() {
  HEAP_ALLOCATOR = (Allocator){.alloc = heap_allocator_alloc, .dealloc = heap_allocator_dealloc};
  GLOBAL_BUMP_ALLOCATOR = (Allocator){.alloc = bump_allocator_alloc, .dealloc = bump_allocator_dealloc};

  uint8_t *buffer = malloc(GLOBAL_BUMP_CAPACITY);
  bump_init(&GLOBAL_BUMP, buffer, GLOBAL_BUMP_CAPACITY);
}
