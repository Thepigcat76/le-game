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
#include <raylib.h>

#define TICK_RATE 20                     // ticks per second
#define TICK_INTERVAL (1.0f / TICK_RATE) // seconds per tick

#define MAX_TICKS_PER_FRAME 20

typedef struct _game {
  MenuId cur_menu;
  bool paused;
  // SAVES
  SaveDescriptor *saves;
  // SAVE SPECIFIC
  Save cur_save;
  // pointers to the fields in the current save
  Player *player;
  World *world;
  // RENDERING
  ParticleManager particle_manager;
  UiRenderer ui_renderer;
  Window window;
  // ASSET MANAGEMENT
  SoundManager sound_manager;
  ShaderManager shader_manager;
  TextureManager texture_manager;
  // TILE CATEGORIES
  TileCategoryLookup tile_category_lookup;
  // DEBUGGING
  DebugOptions debug_options;
  // World Rendering
  RenderTexture2D world_texture;
  // KEYS PRESSED
  PressedKeys pressed_keys;
  bool slot_selected;
} Game;

extern Game GAME;
extern Music MUSIC;

// GAME CREATION

// Creates a new directory for this save and the config file
// Creates a new save and stores it in GAME.cur_save
void game_create_save(Game *game, const char *save_name, const char *seed_lit);

// Load Save Descriptors to GAME.saves
void game_load_saves(Game *game);

// Load the specificed save to GAME.cur_save
void game_load_save(Game *game, SaveDescriptor desc);

// Unloads GAME.cur_save by saving all game data to disk
// Performs memory cleanup for some things like the item container bump
void game_unload_save(Game *game);

void game_feature_add(Game *game, GameFeature game_feature);

// Creates a world as described in GAME.cur_save
void game_create_save_world(Game *game);

// Returns a pointer to an array of two strings (first one being the adjective, second one the noun)
char **game_save_name_random(Game *game);

void game_feature_create(Game *game);

void game_reload(Game *game);

void game_tick(Game *game);

// GAME RENDER

void game_render(Game *game, float alpha);

void game_render_overlay(Game *game);

// GAME LOAD/SAVE

// Loads the described save and stores it in GAME.cur_save
void game_load_save_data(Game *game, SaveDescriptor desc);

void game_save_save_data(Game *game, Save *save);

// MANAGMENT

// PRE/POST GAME INITIALIZATION
void game_begin(void);

void game_end(void);

// GAME INIT/DEINIT

void game_init(Game *game);

void game_deinit(Game *game);

// MENUS

void game_init_menu(Game *game);

bool game_cur_menu_hides_game(Game *game);

void game_render_menu(Game *game);

void game_set_menu(Game *game, MenuId menu_id);

// PARTICLES

ParticleInstance *game_emit_particle_ex(Game *game, ParticleInstance particle_instance);

ParticleInstance *game_emit_particle(Game *game, int x, int y, ParticleId particle_id,
                                     ParticleInstanceEx particle_extra);

void game_render_particle(Game *game, ParticleInstance particle, bool behind_player);

void game_render_particles(Game *game, bool behind_player);
