#include "../../include/textures.h"
#include "../../include/assets.h"
#include "../../include/shared.h"
#include "lilc/array.h"
#include "lilc/log.h"
#include <lilc/bump.h>
#include <raylib.h>

AssetId TEX_IDS[_amount_texture_handles];

static AssetId tex_asset_id(AssetManager *m, char *filename) {
  cw_Texture *tex;
  array_foreach(m->textures, tex) {
    if (str_eq(tex->path, TextFormat(ASSETS_DIR TEXTURES_DIR "/%s.png", filename))) {
      return tex->id;
    }
  }
  return TEX_IDS[TEX_ERR];
}

void texture_handles_assign_id(AssetManager *m) {
  TEX_IDS[TEX_ERR] = tex_asset_id(m, "err_texture");

  TEX_IDS[TEX_BRICKS] = tex_asset_id(m, "bricks");
  TEX_IDS[TEX_BUSH] = tex_asset_id(m, "bush");
  TEX_IDS[TEX_CHEST] = tex_asset_id(m, "chest");
  TEX_IDS[TEX_DIRT] = tex_asset_id(m, "dirt");
  TEX_IDS[TEX_DUNGEON_FLOOR] = tex_asset_id(m, "dungeon_floor");
  TEX_IDS[TEX_DUNGEON_PORTAL] = tex_asset_id(m, "dungeon_portal");
  TEX_IDS[TEX_GRASS_TILES] = tex_asset_id(m, "grass_tiles");
  TEX_IDS[TEX_STONE_TILES] = tex_asset_id(m, "stone_tiles");
  TEX_IDS[TEX_LIGHTER_DIRT_TILES] = tex_asset_id(m, "lighter_dirt_tiles");
  TEX_IDS[TEX_GRASS] = tex_asset_id(m, "grass");
  TEX_IDS[TEX_OVEN] = tex_asset_id(m, "oven");
  TEX_IDS[TEX_TALL_OVEN] = tex_asset_id(m, "tall_oven");
  TEX_IDS[TEX_STONE] = tex_asset_id(m, "stone");
  TEX_IDS[TEX_TREE] = tex_asset_id(m, "tree_5");
  TEX_IDS[TEX_TORCH] = tex_asset_id(m, "torch");
  TEX_IDS[TEX_WATER] = tex_asset_id(m, "water");
  TEX_IDS[TEX_WORKSTATION] = tex_asset_id(m, "workstation");

  TEX_IDS[TEX_AXE] = tex_asset_id(m, "axe");
  TEX_IDS[TEX_PICKAXE] = tex_asset_id(m, "pickaxe");
  TEX_IDS[TEX_SHOVEL] = tex_asset_id(m, "shovel");
  TEX_IDS[TEX_HAMMER] = tex_asset_id(m, "hammer");
  TEX_IDS[TEX_BACKPACK] = tex_asset_id(m, "backpack");
  TEX_IDS[TEX_HEAL_POTION] = tex_asset_id(m, "heal_potion");
  TEX_IDS[TEX_MAP] = tex_asset_id(m, "map");
  TEX_IDS[TEX_STICK] = tex_asset_id(m, "stick");

  TEX_IDS[TEX_GRASS_INV] = tex_asset_id(m, "grass_inv");
  TEX_IDS[TEX_STONE_INV] = tex_asset_id(m, "stone_inv");
  TEX_IDS[TEX_DIRT_INV] = tex_asset_id(m, "dirt_inv");

  TEX_IDS[TEX_MAP_ICON_HOUSE] = tex_asset_id(m, "map_icon_house");
  TEX_IDS[TEX_MAP_ICON_TREE] = tex_asset_id(m, "map_icon_tree");

  TEX_IDS[TEX_PARTICLE] = tex_asset_id(m, "particle");
  TEX_IDS[TEX_WALK_PARTICLES] = tex_asset_id(m, "walk_particles");

  TEX_IDS[TEX_PLAYER_BACK_WALK] = tex_asset_id(m, "player_back_walk");
  TEX_IDS[TEX_PLAYER_FRONT_WALK] = tex_asset_id(m, "player_front_walk");
  TEX_IDS[TEX_PLAYER_LEFT_WALK] = tex_asset_id(m, "player_left_walk");
  TEX_IDS[TEX_PLAYER_RIGHT_WALK] = tex_asset_id(m, "player_right_walk");

  TEX_IDS[TEX_PLAYER_BACK] = tex_asset_id(m, "player_back");
  TEX_IDS[TEX_PLAYER_FRONT] = tex_asset_id(m, "player_front");
  TEX_IDS[TEX_PLAYER_LEFT] = tex_asset_id(m, "player_left");
  TEX_IDS[TEX_PLAYER_RIGHT] = tex_asset_id(m, "player_right");

  TEX_IDS[TEX_WATER_OVERLAY] = tex_asset_id(m, "water_overlay");

  TEX_IDS[TEX_BUILDING_SHOP] = tex_asset_id(m, "building_shop");

  TEX_IDS[TEX_BREAKING_OVERLAY] = tex_asset_id(m, "breaking_overlay");
  TEX_IDS[TEX_CURSOR_FIST] = tex_asset_id(m, "cursor_fist");
  TEX_IDS[TEX_CURSOR] = tex_asset_id(m, "cursor");
  TEX_IDS[TEX_TOOLTIP] = tex_asset_id(m, "ui/tooltip");

  TEX_IDS[TEX_BUTTON] = tex_asset_id(m, "ui/button");
  TEX_IDS[TEX_BUTTON_SELECTED] = tex_asset_id(m, "ui/button_selected");
  TEX_IDS[TEX_SAVE_SLOT_BUTTTON] = tex_asset_id(m, "ui/save_slot");
  TEX_IDS[TEX_SAVE_SLOT_BUTTON_SELECTED] = tex_asset_id(m, "ui/save_slot_selected");
  TEX_IDS[TEX_TEXT_INPUT] = tex_asset_id(m, "ui/text_input");

  TEX_IDS[TEX_BUTTON_BACK_TO_GAME] = tex_asset_id(m, "ui/buttons/back_to_game");
  TEX_IDS[TEX_BUTTON_SELECTED_BACK_TO_GAME] = tex_asset_id(m, "ui/buttons/selected_back_to_game");
  TEX_IDS[TEX_BUTTON_GAME_SETTINGS] = tex_asset_id(m, "ui/buttons/game_settings");
  TEX_IDS[TEX_BUTTON_SELECTED_GAME_SETTINGS] = tex_asset_id(m, "ui/buttons/selected_game_settings");
  TEX_IDS[TEX_BUTTON_CLIENT_SETTINGS] = tex_asset_id(m, "ui/buttons/client_settings");
  TEX_IDS[TEX_BUTTON_SELECTED_CLIENT_SETTINGS] = tex_asset_id(m, "ui/buttons/selected_client_settings");
  TEX_IDS[TEX_BUTTON_LEAVE_GAME] = tex_asset_id(m, "ui/buttons/leave_game");
  TEX_IDS[TEX_BUTTON_SELECTED_LEAVE_GAME] = tex_asset_id(m, "ui/buttons/selected_leave_game");

  TEX_IDS[TEX_MAIN_HAND_SLOT] = tex_asset_id(m, "ui/main_hand_slot");
  TEX_IDS[TEX_SLOT] = tex_asset_id(m, "ui/slot");
}

i32 cw_texture_load(cw_Texture *texture, AssetManager *manager, FileEntry file_entry) {
  Texture2D tex = LoadTexture(file_entry.full_path);
  if (tex.id == 0) {
    log_error("Failed to load texture %s", file_entry.full_path);
    return 0;
  }
  
  texture->texture = tex;
  texture->width = tex.width;
  texture->height = tex.height;
  texture->path = str_cpy(file_entry.full_path, &manager->asset_bump_allocator);

  return 1;
}

void cw_texture_unload(cw_Texture *texture) {
  UnloadTexture(texture->texture);
}
