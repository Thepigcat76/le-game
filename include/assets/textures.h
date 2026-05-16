#pragma once

#include <stdlib.h>
#include "../shared.h"
#include "raylib.h"

/* VARIANT TEXTURES */

typedef struct {
  AssetId *variants;
} SingleTileVariant;

typedef struct {
  Vec2i *cases;
  SingleTileVariant *case_variant;
} ConnectedTileVariant;

typedef struct {
  // Links to the base asset. In the case of dirt this would be 'TEX_DIRT',
  // which in turn links to the AssetIds of its variants
  AssetId id;
  enum {
    TEX_VAR_NONE,
    TEX_VAR_SINGLE,
    TEX_VAR_CONNECTED,
  } kind;
  AssetId *variants;
} VariantTexture;

typedef enum {
  // Tile textures
  TEX_BRICKS,
  TEX_BUSH,
  TEX_CHEST,
  TEX_DIRT,
  TEX_DUNGEON_FLOOR,
  TEX_DUNGEON_PORTAL,
  TEX_GRASS_TILES,
  TEX_STONE_TILES,
  TEX_LIGHTER_DIRT_TILES,
  TEX_GRASS,
  TEX_OVEN,
  TEX_TALL_OVEN,
  TEX_STONE,
  TEX_TREE,
  TEX_TORCH,
  TEX_WATER,
  TEX_WORKSTATION,
  // Item textures
  TEX_AXE,
  TEX_PICKAXE,
  TEX_SHOVEL,
  TEX_HAMMER,
  TEX_BACKPACK,
  TEX_HEAL_POTION,
  TEX_MAP,
  TEX_STICK,
  // Inv textures
  TEX_GRASS_INV,
  TEX_STONE_INV,
  TEX_DIRT_INV,
  // Ui textures
  TEX_MAP_SPRITE,
  TEX_BUTTON,
  TEX_BUTTON_SELECTED,
  TEX_SAVE_SLOT_BUTTTON,
  TEX_SAVE_SLOT_BUTTON_SELECTED,
  TEX_TEXT_INPUT,

  TEX_BUTTON_BACK_TO_GAME,
  TEX_BUTTON_SELECTED_BACK_TO_GAME,
  TEX_BUTTON_GAME_SETTINGS,
  TEX_BUTTON_SELECTED_GAME_SETTINGS,
  TEX_BUTTON_CLIENT_SETTINGS,
  TEX_BUTTON_SELECTED_CLIENT_SETTINGS,
  TEX_BUTTON_LEAVE_GAME,
  TEX_BUTTON_SELECTED_LEAVE_GAME,

  TEX_MAIN_HAND_SLOT,
  TEX_SLOT,
  // Map textures
  TEX_MAP_ICON_HOUSE,
  TEX_MAP_ICON_TREE,
  // Particle
  TEX_PARTICLE,
  TEX_WALK_PARTICLES,
  // Player textures
  TEX_PLAYER_BACK_WALK,
  TEX_PLAYER_FRONT_WALK,
  TEX_PLAYER_LEFT_WALK,
  TEX_PLAYER_RIGHT_WALK,
  
  TEX_PLAYER_BACK,  
  TEX_PLAYER_FRONT,  
  TEX_PLAYER_LEFT,  
  TEX_PLAYER_RIGHT,

  TEX_WATER_OVERLAY,
  // Building textures
  TEX_BUILDING_SHOP,

  // Misc textures
  TEX_BREAKING_OVERLAY,
  TEX_CURSOR_FIST,
  TEX_CURSOR,
  TEX_TOOLTIP,

  TEX_ERR,
  _amount_texture_handles,
} TextureHandle;

extern AssetId TEX_IDS[_amount_texture_handles];
