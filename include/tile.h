#pragma once

#include "assets.h"
#include "data.h"
#include "game_feature.h"
#include "item.h"
#include "raylib.h"
#include "shared.h"
#include "textures.h"
#include "category.h"
#include "tile/tile_props.h"
#include "tile/adv_tile.h"
#include "registries/tiles.h"

#define DEFAULT_TILE_DIMENSIONS dimf(16, 16)

extern AdvTileInstance *ADV_TILES;

TileLayer tile_layer_from_str(const char *layer_literal);

void tile_instance_debug(const TileInstance *tile, char *buf);

Rectf tile_collision_box_at(const TileInstance *tile, i32 x, i32 y);

Dimensionsf tile_collision_dimensions_at(const TileInstance *tile);

Vec2f tile_collision_offset_at(const TileInstance *tile);

void tile_render(TileInstance *tile, i32 x, i32 y, bool dbg);

void tile_render_scaled(TileInstance *tile, i32 x, i32 y, float scale);

void tile_right_click(TileInstance *tile);

TileInstance tile_break_remainder(const TileInstance *tile, TilePos pos);

void tile_tick(TileInstance *tile);
