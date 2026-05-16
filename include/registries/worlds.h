#pragma once

typedef enum {
  WORLD_BASE,
  WORLD_DUNGEON_TEST,

  _amount_world_ids,
} WorldId;

typedef struct {
  WorldId id;
} WorldProperties;
