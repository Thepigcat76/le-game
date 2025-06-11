#include "../../include/bump.h"

void bump_init(Bump *bump, void *buffer, size_t capacity) {
  bump->buffer = buffer;
  bump->capacity = capacity;
  bump->offset = 0;
}

void *bump_alloc(Bump *bump, size_t bytes) {
  if (bump->offset + bytes > bump->capacity) {
    return NULL;
  }

  void *ptr = bump->buffer + bump->offset;
  bump->offset += bytes;
  return ptr;
}

void bump_reset(Bump *bump) {
    bump->offset = 0;
}
