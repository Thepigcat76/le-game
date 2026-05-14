#pragma once

#include "debug.h"
#include "item.h"
#include "save.h"
#include "space_desc.h"
#include "tile.h"
#include "category.h"
#include "camera.h"

#define MAX_TICKS_PER_FRAME 20

typedef struct _game {
  // SAVE SPECIFIC
  Save cur_save;
  // pointers to the fields in the current save
  // CATEGORIES
  Category tile_categories;
  Category item_categories;
  // DEBUGGING
  Debug debug;
  // Client/Server - depending on
  // the side we are on either one
  // of these is not NULL
  struct _client_game *client_game;
  struct _server_game *server_game;
  bool save_loaded;
} Game;

typedef enum {
  SIDE_CLIENT,
  SIDE_SERVER,
} GameSide;

extern GameSide GAME_SIDE;

// Initialize registries
void game_registry_setup(void);

void game_categories_setup(Game *game);

// GAME CREATION

// Creates a new directory for this save and the config file
// Creates a new save and stores it in GAME.cur_save
void game_create_save(Game *game, SaveDescriptor save_desc);

// Load the specificed save to GAME.cur_save from disk
void game_load_save(Game *game, SaveDescriptor desc);

// Loads files in the save directory to GAME.local_saves
void game_load_saves(Game *game);

// Unloads GAME.cur_save by saving all game data to disk
// Does some memory cleaning like reseting the item container bump
void game_unload_save(Game *game);

void game_feature_add(Game *game, GameFeature game_feature);

// Creates a world as described in GAME.cur_save
void game_create_save_world(Game *game);

// Returns a pointer to an array of two strings (first one being the adjective, second one the noun)
char **game_save_name_random(Game *game);

void game_feature_create(Game *game);

void game_tick(Game *game);

// Tile category/breaking

TileCategories item_tile_categories(const ItemInstance *item);

bool item_tool_correct_for_tile(const ItemInstance *item, const TileInstance *tile, Category *tile_categories);

// GAME SPACES

void game_enter_space(Game *game, SpaceDescriptor desc);

// GAME LOAD/SAVE

// Loads the described save and stores it in GAME.cur_save
void game_load_save_data(Game *game, SaveDescriptor desc);

void game_save_save_data(Game *game, Save *save);

// GAME INIT/DEINIT

void game_init(Game *game);

void game_deinit(Game *game);
