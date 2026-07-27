#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "registries/spaces.h"

typedef struct {
  SpaceId space_id;
  size_t *disk_ids;
} SpaceIdLookupEntry;

// TODO: Legacy stuff, ngl
// Lookup for the disk id of a specific space
typedef struct {
  SpaceIdLookupEntry *entries;
} SpaceIdLookup;

typedef struct {
  SpaceId space_id;
  size_t id;
  bool external;
} SpaceDescriptor;