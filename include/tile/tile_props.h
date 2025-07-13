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

#define TILE_TEXTURE_PROPS_DEFAULT ((TileTextureProperties){.has_variants = false, .uses_tileset = false})

#define TILE_PROPS_DEFAULT ((TileProperties){.disable_collisions = false, .tile_color = RAYWHITE, .break_time = 64})

#define TILE_PROPS_DEFAULT_WITH_COLOR(color)                                                                           \
{.disable_collisions = false, .tile_color = color, .break_time = 64}
