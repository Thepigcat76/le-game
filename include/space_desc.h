#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "world_type.h"

typedef enum {
  SPACE_BASE,
  SPACE_DUNGEON_TEST,
} SpaceId;

typedef struct {
  SpaceId space_id;
  const WorldType *world_type;
  // Whether this space should be cached, so we dont have to load it from disk each time
  // Useful for spaces that are visited often, SPACE_BASE, last couple of dungeons, houses
  // that are visited often...
  bool cached;
  // Whether this space is the default space, SPACE_BASE is the default space
  bool default_space;
  // Global = true ? only one space of this type can exist : multiple spaces of this type can be created
  bool global;
} SpaceType;

typedef struct {
  SpaceId space_id;
  size_t *disk_ids;
} SpaceIdLookupEntry;

// Lookup for the disk id of a specific space
typedef struct {
  SpaceIdLookupEntry *entries;
} SpaceIdLookup;

extern SpaceType *SPACES;

void space_types_init(void);

char *space_id_to_name(SpaceId id);

SpaceId space_id_by_name(const char *name);

typedef struct {
  const SpaceType *type;
  size_t id;
} SpaceDescriptor;