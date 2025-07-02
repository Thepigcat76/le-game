#pragma once

#include "being.h"
#include "data.h"
#include "debug.h"
#include "item.h"
#include "keys.h"
#include "menu.h"
#include "particle.h"
#include "save.h"
#include "shaders.h"
#include "shared.h"
#include "sounds.h"
#include "tile.h"
#include "window.h"
#include "ui.h"
#include "net/client.h"
#include "net/server.h"

#define MAX_TICKS_PER_FRAME 20

typedef struct _game {
  // SAVE SPECIFIC
  Save cur_save;
  // pointers to the fields in the current save
  Player *player;
  World *world;
  // TILE CATEGORIES
  TileCategoryLookup tile_category_lookup;
  // DEBUGGING
  DebugOptions debug_options;
  // KEYS PRESSED
  PressedKeys pressed_keys;
  bool slot_selected;
  // Client/Server - depending on
  // the side we are on either one
  // of these is not NULL
  ClientGame *client_game;
  ServerGame *server_game;
} Game;

extern Game GAME;

// Initializes the game
void game_create(Game *game);

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

void game_reload(Game *game);

void game_tick(Game *game);

// GAME LOAD/SAVE

// Loads the described save and stores it in GAME.cur_save
void game_load_save_data(Game *game, SaveDescriptor desc);

void game_save_save_data(Game *game, Save *save);

// GAME INIT/DEINIT

void game_init(Game *game);

void game_deinit(Game *game);
