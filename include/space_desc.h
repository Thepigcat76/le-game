#pragma once

#include <stdbool.h>

typedef enum {
  SPACE_BASE,
} SpaceId;

char *space_id_to_name(SpaceId id);

SpaceId space_id_by_name(const char *name);

typedef struct {
  SpaceId space_id;
  // Whether the space should be kept around in memory
  bool lazy_loaded;
} SpaceDescriptor;