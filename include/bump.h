#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t *buffer;
  size_t capacity;
  size_t offset;
} Bump;

void bump_init(Bump *bump, void *buffer, size_t capacity);

void *bump_alloc(Bump *bump, size_t bytes);

void bump_reset(Bump *bump);
