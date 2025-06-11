#pragma once

#include "tile.h"
#include "tile_props.h"

struct _tile_props {
  TileProperties tile_props;
  TileTextureProperties texture_props;
};

#define TILE_TYPE_INIT(tile_id, tile_layer, item_type_ptr, texture_path, ...)                                          \
  {                                                                                                                    \
    bool has_texture = texture_path != NULL;                                                                           \
    AdvTexture texture = has_texture ? adv_texture_load("res/assets/" texture_path ".png") : (AdvTexture){0};          \
    struct _tile_props props = (struct _tile_props){__VA_ARGS__};                                                      \
    TILES[tile_id] = (TileType){                                                                                       \
        .id = tile_id,                                                                                                 \
        .layer = tile_layer,                                                                                           \
        .has_texture = has_texture,                                                                                    \
        .texture = texture,                                                                                            \
        .tile_dimensions = has_texture                                                                                 \
            ? (props.texture_props.uses_tileset ? dimf(TILE_SIZE, TILE_SIZE) : dimf(texture.width, texture.height))    \
            : dimf(0, 0),                                                                                              \
        .tile_item = item_type_ptr,                                                                                    \
        .tile_props = props.tile_props,                                                                                \
        .texture_props = props.texture_props};                                                                         \
    TILES_AMOUNT++;                                                                                                    \
  }
