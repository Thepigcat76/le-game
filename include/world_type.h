#pragma once

typedef enum {
  WORLD_BASE,
  WORLD_DUNGEON_TEST,
} WorldId;

typedef struct {
  WorldId id;
} WorldType;

extern WorldType *WORLD_TYPES;

void world_types_init(void);
