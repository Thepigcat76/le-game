#pragma once

#include "../net/client.h"
#include "../tile.h"
#include "lilc/array.h"
#include "tile_props.h"

struct _tile_props {
  TileProperties tile_props;
  TileTextureProperties texture_props;
  u16 width;
  u16 height;
};

#define TILE_TYPE_INIT(tile_id, tile_layer, item_type_ptr, name, asset_id, ...)                                                            \
  do {                                                                                                                                     \
    bool has_texture = asset_id != -1;                                                                                                     \
    struct _tile_props props = (struct _tile_props){__VA_ARGS__};                                                                          \
    TileType t = (TileType){.id = tile_id,                                                                                                 \
                            .layer = tile_layer,                                                                                           \
                            .has_texture = has_texture,                                                                                    \
                            .texture = asset_id,                                                                                           \
                            .tile_dimensions = has_texture                                                                                 \
                                ? (props.texture_props.uses_tileset                                                                        \
                                       ? dimf(TILE_SIZE, TILE_SIZE)                                                                        \
                                       : dimf(props.width == 0 ? 16 : props.width, props.height == 0 ? 16 : props.height))                 \
                                : dimf(0, 0),                                                                                              \
                            .tile_item = item_type_ptr,                                                                                    \
                            .tile_props = props.tile_props,                                                                                \
                            .texture_props = props.texture_props};                                                                         \
    array_set(TILES, tile_id, t);                                                                                                          \
    TILES_AMOUNT++;                                                                                                                        \
  } while (0)
