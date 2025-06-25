#pragma once

#include "raylib.h"

typedef struct {
  bool has_variants;
  bool uses_tileset;
} TileTextureProperties;

typedef struct {
  bool disable_collisions;
  int break_time;
  Color tile_color;
} TileProperties;

#define TILE_PROPS_DEFAULT ((TileProperties){.disable_collisions = false, .tile_color = WHITE, .break_time = 64})

#define TILE_PROPS_DEFAULT_WITH_COLOR(color)                                                                           \
{.disable_collisions = false, .tile_color = color, .break_time = 64}
