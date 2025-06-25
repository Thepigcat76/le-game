#pragma once

#include "being.h"
#include "data.h"
#include "debug.h"
#include "game_feature.h"
#include "item.h"
#include "menu.h"
#include "particle.h"
#include "player.h"
#include "shaders.h"
#include "shared.h"
#include "sounds.h"
#include "tile.h"
#include "ui.h"
#include "keys.h"
#include "world.h"
#include <raylib.h>

#define TICK_RATE 20                     // ticks per second
#define TICK_INTERVAL (1.0f / TICK_RATE) // seconds per tick

#define MAX_TICKS_PER_FRAME 20

#define TEXTURE_MANAGER_MAX_TEXTURES 64

typedef struct {
  char *save_name;
  float seed;
} GameConfig;

typedef struct {
  int width;
  int height;
  int prev_width;
  int prev_height;
} Window;

typedef enum {
  TEXTURE_CURSOR,
  TEXTURE_TOOLTIP,
  TEXTURE_BREAK,
  TEXTURE_SLOT,
} TextureManagerTexture;

typedef struct {
  Texture2D textures[TEXTURE_MANAGER_MAX_TEXTURES];
} TextureManager;

typedef struct _game {
  MenuId cur_menu;
  bool paused;
  int cur_save;
  // SAVE SPECIFIC
  Player player;
  World world;
  GameFeatureStore feature_store;
  // LOADING
  int detected_saves;
  GameConfig *save_configs;
  // RENDERING
  ParticleManager particle_manager;
  UiRenderer ui_renderer;
  Window window;
  // SOUNDS
  SoundManager sound_manager;
  // SHADERS
  ShaderManager shader_manager;
  // MISC TEXTURES
  TextureManager texture_manager;
  // TILE CATEGORIES
  TileCategoryLookup tile_category_lookup;
  // DEBUGGING
  DebugOptions debug_options;
  // World Rendering
  RenderTexture2D world_texture;
  // KEYS PRESSED
  PressedKeys pressed_keys;
  float tick_delta;
  bool slot_selected;
} Game;

extern Game GAME;
extern Music MUSIC;

// GAME CREATION

void game_feature_add(Game *game, GameFeature game_feature);

void game_create_world(Game *game, float seed);

void game_create_save(Game *game, const char *save_name, const char *seed_lit);

void game_detect_saves(Game *game);

// Returns a pointer to an array of two strings (first one being the adjective, second one the noun)
char **game_save_name_random(Game *game);

void game_feature_create(Game *game);

void game_reload(Game *game);

void game_cur_save_init(Game *game);

void game_tick(Game *game);

// GAME RENDER

void game_render(Game *game, float alpha);

void game_render_overlay(Game *game);

// GAME LOAD/SAVE

void game_load_cur_save(Game *game);

void game_save_cur_save(Game *game);

void game_load(Game *game);

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
