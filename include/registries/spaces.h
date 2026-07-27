#pragma once

typedef enum {
  SPACE_BASE,
  SPACE_DUNGEON_TEST,

  _amount_space_ids,
} SpaceId;

typedef struct {
  SpaceId id;

  const char *ident;
  const char *name;

  // Whether this space should be cached, so we dont have to load it from disk each time
  // Useful for spaces that are visited often, SPACE_BASE, last couple of dungeons, houses
  // that are visited often...
  bool cached;
  // Whether this space is the default space, SPACE_BASE is the default space
  bool default_space;
  // Global = true ? only one space of this type can exist : multiple spaces of this type can be created
  bool global;
} SpaceProperties;

SpaceId space_id_by_ident(const char *ident);
