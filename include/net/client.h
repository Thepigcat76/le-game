#pragma once

#include "../game.h"
#include "../keys.h"
#include "../menu.h"
#include "../particle.h"
#include "../player.h"
#include "../save.h"
#include "../sounds.h"
#include "../textures.h"
#include "../ui.h"
#include "../world.h"
#include "packet.h"
#include "queue.h"
#include "sockets.h"
#include "../assets.h"

typedef struct {
  bool paused;
  MenuId cur_menu;

  bool slot_selected;
  TileInstance *hovered_tile;
  BeingInstance *hovered_being;

  PressedKeys pressed_keys;
} ClientState;

typedef struct _client_game {
  Camera2D cam;

  ClientState state;

  bool singleplayer;
  
  bool initializing;
  
  /* Managers */
  AssetManager asset_manager;
  // Textures like animated textures
  TextureManager tex_manager;

  SoundManager sound_manager;
  ParticleManager particle_manager;

  UiRenderer ui_renderer;
  Window window;
  RenderTexture2D world_texture;
  // Saves
  // These are saves that are actually stored on disk
  SaveDescriptor *local_saves;
  // Main game
  Game game;
  // World, Player - points to the
  // world and player usually stored
  // in CLIENT_GAME.cur_save
  World *world;
  PlayerRenderDescriptor *players;
  Player cur_player;
  i32 player_id;
  addr_t server_addr;
  bool connected_to_server;
} ClientGame;

typedef struct network_connection {
  addr_t server_addr;
  bool connected;
  PacketQueue queue;

  Bump packet_bump;
  Allocator packet_bump_allocator;
} NetworkConnection;

extern ClientGame CLIENT_GAME;
extern NetworkConnection CLIENT_CONNECTION;
extern pthread_mutex_t CLIENT_MUTEX;
extern pthread_cond_t CLIENT_COND;

#define CLIENT_PLAYER (&CLIENT_GAME.cur_player)

#define CLIENT_WORLD (CLIENT_GAME.world)

void client_start(void);

void client_init(ClientGame *game);

void client_deinit(ClientGame *game);

void client_setup_raylib(void);

void client_deinit_raylib(void);

// Saves

/* Needs to be called after the world is loaded on common
   to notify the client that the world and player need to
   be rendered */
void client_init_loaded_save(ClientGame *game, Save *save);

// TICKING

void client_tick(ClientGame *game);

// MENUS

void client_init_menu(ClientGame *game);

bool client_menu_hides_game(ClientGame *game, MenuId menu);

bool client_menu_is_container(ClientGame *game, MenuId menu);

void client_render_menu(ClientGame *game);

void client_set_menu(ClientGame *game, MenuId menu_id);

// GAME RENDER

void client_render(ClientGame *game, float alpha);

void client_render_overlay(ClientGame *game);

// CURSOR

bool cursor_can_interact_with_tile(ClientGame *game, TileInstance *tile);

bool cursor_can_interact_with_being(ClientGame *game, BeingInstance *being);

// PARTICLES

void client_render_particle(ClientGame *game, ParticleInstance particle, bool behind_player);

void client_render_particles(ClientGame *game, bool behind_player);

ParticleInstance *client_emit_particle_ex(ClientGame *game, ParticleInstance particle_instance);

ParticleInstance *client_emit_particle(ClientGame *game, int x, int y, ParticleId particle_id, ParticleInstanceEx particle_extra);

// Networking

addr_t client_join_server(ClientGame *game, const char *ip_addr, uint32_t port);

void client_leave_server(ClientGame *game);
