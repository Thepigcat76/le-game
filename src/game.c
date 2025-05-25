#include "../include/game.h"
#include "../include/config.h"
#include "raylib.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define RELOAD(src_file_prefix)                                                                                        \
  extern void src_file_prefix##_on_reload();                                                                           \
  src_file_prefix##_on_reload();

#define RENDER_MENU(ui_renderer, menu_name)                                                                            \
  extern void menu_name##_render(UiRenderer *renderer, const Game *game);                                              \
  menu_name##_render(ui_renderer, game);

#define INIT_MENU(menu_name)                                                                                           \
  extern void menu_name##_init();                                                                                      \
  menu_name##_init();

void game_reload() {
  RELOAD(tile);
  RELOAD(config);
}

Game GAME;

void game_create_world(Game *game, float seed) {
  player_set_pos_ex(&game->player, TILE_SIZE * ((float)CHUNK_SIZE / 2), TILE_SIZE * ((float)CHUNK_SIZE / 2), false,
                    false, false);
  game->world.seed = seed;
  world_gen(&game->world);
}

void game_detect_saves(Game *game) {
  game->detected_saves = 0;
  DIR *dir = opendir(SAVE_DIR);

  if (dir == NULL) {
    perror("Failed to open save directory to load saves");
    exit(1);
  }

  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    const char *full_dir_name = TextFormat("%s%s", SAVE_DIR, entry->d_name);

    TraceLog(LOG_DEBUG, "Dir entry: %s", full_dir_name);
    if (is_dir(full_dir_name) && string_starts_with(entry->d_name, "save")) {
      game->detected_saves++;
    }
  }

  closedir(dir);
}

// TICKING

void game_tick(Game *game) {
  if (!game->paused) {
    bool zoom_in = IsKeyDown(KEY_UP);
    bool zoom_out = IsKeyDown(KEY_DOWN);

    player_handle_zoom(&game->player, zoom_in, zoom_out);

    bool w = IsKeyDown(KEYBINDS.move_backward_key);
    bool a = IsKeyDown(KEYBINDS.move_left_key);
    bool s = IsKeyDown(KEYBINDS.move_foreward_key);
    bool d = IsKeyDown(KEYBINDS.move_right_key);

    player_handle_movement(&game->player, w, a, s, d);
  }
}

// RENDERING

void game_render(Game *game) {
  world_render_layer(&game->world, TILE_LAYER_GROUND);

  world_render_layer_top_split(&game->world, &game->player, true);

  for (int i = 0; i < game->world.beings_amount; i++) {
    being_render(&game->world.beings[i]);
  }

  game_render_particles(game, true);

  player_render(&game->player);

  world_render_layer_top_split(&game->world, &game->player, false);
}

void game_render_overlay(Game *game) {
  Vec2i pos = vec2i(SCREEN_WIDTH - (3.5 * 16) - 30, (SCREEN_HEIGHT / 2.0f) - (3.5 * 8));
  DrawTextureEx(MAIN_HAND_SLOT_TEXTURE, (Vector2){pos.x, pos.y}, 0, 4.5, WHITE);
  item_render(&game->player.held_item, pos.x + 2 * 3.5, pos.y + 2 * 3.5);

#ifdef SURTUR_DEBUG
  tile_render_scaled(&game->debug_options.selected_tile_to_place_instance, 4);
#endif

  game_render_menu(game);
}

// UI/MENUS

bool game_cur_menu_hides_game(Game *game) {
  return game->cur_menu == MENU_START || game->cur_menu == MENU_NEW_SAVE || game->cur_menu == MENU_LOAD_SAVE;
}

void game_init_menu(Game *game) {
  INIT_MENU(save_menu);
  // INIT_MENU(start_menu);
  // INIT_MENU(debug_menu);
}

void game_render_menu(Game *game) {
  UiRenderer *ui_renderer = &game->ui_renderer;
  switch (game->cur_menu) {
  case MENU_SAVE: {
    RENDER_MENU(ui_renderer, save_menu);
    break;
  }
  case MENU_START: {
    RENDER_MENU(ui_renderer, start_menu);
    break;
  }
  case MENU_BACKPACK: {
    RENDER_MENU(ui_renderer, backpack_menu);
    break;
  }
  case MENU_DEBUG: {
    RENDER_MENU(ui_renderer, debug_menu);
    break;
  }
  case MENU_NEW_SAVE: {
    RENDER_MENU(ui_renderer, new_save_menu);
    break;
  }
  case MENU_LOAD_SAVE: {
    RENDER_MENU(ui_renderer, load_save_menu);
    break;
  }
  case MENU_NONE: {
    break;
  }
  }
}

static void ui_renderer_calc_height(UiRenderer *ui_renderer) {
  if (ui_renderer->ui_height == -1) {
    ui_renderer->cur_y = 0;
    ui_renderer->simulate = true;
    game_render_menu(&GAME);
    ui_renderer->ui_height = ui_renderer->cur_y;

    ui_renderer->simulate = false;
    ui_renderer->cur_x = 0;
    ui_renderer->cur_y = 0;
  }
}

void game_set_menu(Game *game, MenuId menu_id) {
  game->cur_menu = menu_id;
  ui_renderer_calc_height(&game->ui_renderer);
}

// GAME LOAD/SAVE

// LOAD

static void load_game(Game *game, ByteBuf *bytebuf) {
  Data data_map_0 = byte_buf_read_data(bytebuf);
  DataMap *player_map = &data_map_0.var.data_map;
  player_load(&game->player, player_map);

  Data data_map_1 = byte_buf_read_data(bytebuf);
  DataMap *world_map = &data_map_1.var.data_map;
  load_world(&game->world, world_map);

  data_free(&data_map_0);
  data_free(&data_map_1);
}

void game_load(Game *game) {
  uint8_t bytes[SAVE_DATA_BYTES];
  ByteBuf buf = {.bytes = bytes, .writer_index = 0, .reader_index = 0, .capacity = SAVE_DATA_BYTES};
  byte_buf_from_file(&buf, "save/game.bin");
  load_game(game, &buf);
  TraceLog(LOG_INFO, "Successfully loaded game");
}

// UNLOAD

static void save_game(Game *game, ByteBuf *bytebuf) {
  DataMap player_map = data_map_new(200);
  player_save(&game->player, &player_map);
  DataMap world_map = data_map_new(400);
  save_world(&game->world, &world_map);

  Data player_data = (Data){.type = DATA_TYPE_MAP, .var = {.data_map = player_map}};
  byte_buf_write_data(bytebuf, &player_data);
  Data world_data = (Data){.type = DATA_TYPE_MAP, .var = {.data_map = world_map}};
  byte_buf_write_data(bytebuf, &world_data);

  data_free(&player_data);
  data_free(&world_data);
}

void game_unload(Game *game) {
  tile_variants_free();

  uint8_t bytes[SAVE_DATA_BYTES];
  ByteBuf buf = {.bytes = bytes, .writer_index = 0, .reader_index = 0, .capacity = SAVE_DATA_BYTES};
  save_game(game, &buf);
  byte_buf_to_file(&buf, "save/game.bin");

  free(game->world.chunks);
}

// -- PARTICLES --

ParticleInstance *game_emit_particle_ex(Game *game, ParticleInstance particle_instance) {
  ParticleInstance *particles = GAME.particle_manager.particles;

  for (int i = 0; i < MAX_PARTICLES_AMOUNT; i++) {
    if (!GAME.particle_manager.particles[i].active) {
      float angle = (float)(rand() % 360) * DEG2RAD;
      float speed = 20 + (rand() % 100);
      game->particle_manager.particles[i] = particle_instance;
      return &game->particle_manager.particles[i];
    }
  }
  return NULL;
}

ParticleInstance *game_emit_particle(Game *game, int x, int y, ParticleId particle_id,
                                     ParticleInstanceEx particle_extra) {
  Vector2 pos = {x, y};
  Color particle_color = WHITE;
  if (particle_extra.type == PARTICLE_INSTANCE_TILE_BREAK) {
    particle_color = particle_extra.var.tile_break.tint;
  } else if (particle_extra.type == PARTICLE_INSTANCE_WALKING) {
    particle_color = particle_extra.var.walking.tint;
  }
  ParticleInstance particle_instance = {.position = pos,
                                        .velocity = vec2f(0, 0),
                                        .lifetime = (1.5f + (float)(rand() % 100) / 1000.0f) / 3,
                                        .age = 0,
                                        .color = particle_color,
                                        .active = true,
                                        .id = particle_id,
                                        .extra = particle_extra};
  return game_emit_particle_ex(game, particle_instance);
}

static void particles_update(Game *game) {
  ParticleInstance *particles = GAME.particle_manager.particles;

  float dt = GetFrameTime();
  for (int i = 0; i < MAX_PARTICLES_AMOUNT; i++) {
    if (particles[i].active) {
      particles[i].position.x += particles[i].velocity.x * dt;
      particles[i].position.y += particles[i].velocity.y * dt;

      // particles[i].velocity.y += 100.0f * dt;

      particles[i].age += dt;
      float alpha = 1.0f - (particles[i].age / particles[i].lifetime);
      // particles[i].color.a = (unsigned char)(255 * alpha);

      if (particles[i].age >= particles[i].lifetime) {
        particles[i].active = false;
      }
    }
  }
}

void game_render_particle(Game *game, ParticleInstance particle, bool behind_player) {
  if (particle.active) {
    switch (particle.extra.type) {
    case PARTICLE_INSTANCE_DEFAULT: {
      if (!behind_player) {
        DrawTexture(particle.extra.var.default_texture, particle.position.x, particle.position.y, WHITE);
      }
      break;
    }
    case PARTICLE_INSTANCE_TILE_BREAK: {
      if (!behind_player) {
        DrawTextureV(particle.extra.var.tile_break.texture, (Vector2){particle.position.x, particle.position.y},
                     particle.color);
      }
      break;
    }
    case PARTICLE_INSTANCE_WALKING: {
      if (game->player.direction == DIRECTION_DOWN && behind_player) {
        DrawTextureV(particle.extra.var.walking.texture, (Vector2){particle.position.x, particle.position.y},
                     particle.color);
      } else if (game->player.direction != DIRECTION_DOWN && !behind_player) {
        DrawTextureV(particle.extra.var.walking.texture, (Vector2){particle.position.x, particle.position.y},
                     particle.color);
      }
      break;
    }
    }
  }
}

void game_render_particles(Game *game, bool behind_player) {
  particles_update(game);

  ParticleInstance *particles = GAME.particle_manager.particles;
  for (int i = 0; i < MAX_PARTICLES_AMOUNT; i++) {
    game_render_particle(game, particles[i], behind_player);
  }
}
