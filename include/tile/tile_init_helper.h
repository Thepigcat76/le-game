#pragma once

#include "lilc/array.h"
#include "../tile.h"
#include "../net/client.h"
#include "tile_props.h"

struct _tile_props {
  TileProperties tile_props;
  TileTextureProperties texture_props;
};

#define TILE_TYPE_INIT(tile_id, tile_layer, item_type_ptr, name, asset_id, ...)                                                              \
  do {                                                                                                                                     \
    bool has_texture = asset_id != -1;                                                                                               \
    cw_Texture texture = has_texture ? cw_tex_by_id(&CLIENT_GAME.asset_manager, asset_id) : (cw_Texture){0};                              \
    struct _tile_props props = (struct _tile_props){__VA_ARGS__};                                                                          \
    TileType t = (TileType){.id = tile_id,                                                                                                 \
                            .layer = tile_layer,                                                                                           \
                            .has_texture = has_texture,                                                                                    \
                            .texture = asset_id,                                                                                            \
                            .tile_dimensions = has_texture                                                                                 \
                                ? (props.texture_props.uses_tileset ? dimf(TILE_SIZE, TILE_SIZE) : dimf(texture.width, texture.height))    \
                                : dimf(0, 0),                                                                                              \
                            .tile_item = item_type_ptr,                                                                                    \
                            .tile_props = props.tile_props,                                                                                \
                            .texture_props = props.texture_props};                                                                         \
    array_set(TILES, tile_id, t);                                                                                                          \
    TILES_AMOUNT++;                                                                                                                        \
  } while (0)
