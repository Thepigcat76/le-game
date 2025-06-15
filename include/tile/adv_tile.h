#pragma once

#include "../item/item_container.h"

typedef struct {
    enum {
        ADV_TILE_CHEST,
    } type;
    union {
        struct {
            ItemContainer container;
        } tile_chest;
    } var;
    size_t index;
    TilePos tile_pos;
} AdvTileInstance;
