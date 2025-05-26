#include "../include/game.h"
#include "../include/config.h"
#include "raylib.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define RELOAD(src_file_prefix)                                                                                        \
  extern void src_file_prefix##_on_reload();                                                                           \
  src_file_prefix##_on_reload();

#define RENDER_MENU(ui_renderer, menu_name)                                                                            \
  extern void menu_name##_render(UiRenderer *renderer, const Game *game);                                              \
  menu_name##_render(ui_renderer, game);

#define INIT_MENU(menu_name)                                                                                           \
  extern void menu_name##_init();                                                                                      \
  menu_name##_init();

static void game_init_menu(Game *game);

static bool game_slot_selected();

void game_reload() {
  RELOAD(tile);
  RELOAD(config);
}

Game GAME;
Music MUSIC;

static Sound PLACE_SOUND;

void game_create_save(Game *game, const char *save_name, float seed) {
  create_dir(TextFormat("save/save%d", game->detected_saves));
}

void game_create_world(Game *game, float seed) {
  player_set_pos_ex(&game->player, TILE_SIZE * ((float)CHUNK_SIZE / 2), TILE_SIZE * ((float)CHUNK_SIZE / 2), false,
                    false, false);
  game->world.seed = seed;
  world_gen(&game->world);
}

void game_init(Game *game) {
  game_init_menu(game);

  PLACE_SOUND = LoadSound("res/sounds/place_sound.wav");

  for (int i = 0; i < SOUND_BUFFER_LIMIT; i++) {
    game->sound_manager.sound_buffer[i] = LoadSoundAlias(PLACE_SOUND);
    SetSoundPitch(game->sound_manager.sound_buffer[i], 0.5);
    SetSoundVolume(game->sound_manager.sound_buffer[i], 0.25);
  }

#ifdef SURTUR_DEBUG
  debug_init();
#endif
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

static void handle_tile_interaction(Game *game) {
  Vec2f mouse_pos = GetMousePosition();
  Vec2f mouse_world_pos = GetScreenToWorld2D(mouse_pos, game->player.cam);
  int x_index = floor_div(mouse_world_pos.x, TILE_SIZE);
  int y_index = floor_div(mouse_world_pos.y, TILE_SIZE);

  bool interaction_in_range =
      abs((int)game->player.box.x - x_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE &&
      abs((int)game->player.box.y - y_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE;

  Rectangle slot_rect = {.x = SCREEN_WIDTH - (3.5 * 16) - 30,
                         .y = (SCREEN_HEIGHT / 2.0f) - (3.5 * 8),
                         .width = 20 * 3.5,
                         .height = 20 * 3.5};
  bool slot_selected = CheckCollisionPointRec(GetMousePosition(), slot_rect);

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !slot_selected && interaction_in_range) {
    TileInstance *selected_tile = world_highest_tile_at(&game->world, vec2i(x_index, y_index));
    TileInstance tile = *selected_tile;
    if (CheckCollisionPointRec(mouse_world_pos, selected_tile->box) &&
        (game->player.last_broken_tile.type.layer == selected_tile->type.layer ||
         game->player.last_broken_tile.type.id == TILE_EMPTY)) {
      if (game->player.held_item.type.id == ITEM_HAMMER) {
        for (int y = -1; y <= 1; y++) {
          for (int x = -1; x <= 1; x++) {
            world_remove_tile(&game->world, vec2i(x_index + x, y_index + y));
          }
        }
      } else {
        world_remove_tile(&game->world, vec2i(x_index, y_index));
      }
      game->player.last_broken_tile = tile;
    }
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    game->player.last_broken_tile = TILE_INSTANCE_EMPTY;
  }

  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
    if (game->cur_menu == MENU_DEBUG) {
      DEBUG_GO_TO_POSITION = vec2f(x_index * TILE_SIZE, y_index * TILE_SIZE);
      TraceLog(LOG_DEBUG, "Set target position");
    }
  }

  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !slot_selected && interaction_in_range) {
    TileInstance *selected_tile = world_ground_tile_at(&game->world, vec2i(x_index, y_index));
    if (CheckCollisionPointRec(mouse_world_pos, selected_tile->box)) {
      TileInstance new_tile =
          tile_new(&game->debug_options.selected_tile_to_place_instance.type, x_index * TILE_SIZE, y_index * TILE_SIZE);
      bool placed = world_place_tile(&game->world, vec2i(x_index, y_index), new_tile);
      if (placed && game->sound_manager.sound_timer >= SOUND_COOLDOWN) {
        PlaySound(game->sound_manager.sound_buffer[game->sound_manager.cur_sound++]);
        if (game->sound_manager.cur_sound >= SOUND_BUFFER_LIMIT) {
          game->sound_manager.cur_sound = 0;
        }
        game->sound_manager.sound_timer = 0;
      }
    }
  }
}

static void handle_item_pickup(Game *game) {
  for (int i = 0; i < game->world.beings_amount; i++) {
    if (game->world.beings[i].id == BEING_ITEM &&
        CheckCollisionRecs(game->world.beings[i].context.box, game->player.box)) {
      if (GetTime() - game->world.beings[i].context.creation_time > CONFIG.item_pickup_delay) {
        world_remove_being(&game->world, &game->world.beings[i]);
        break;
      }
    }
  }
}

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

    if (IsKeyReleased(KEYBINDS.open_close_save_menu_key)) {
      if (game->cur_menu == MENU_SAVE) {
        game->cur_menu = MENU_NONE;
        game->paused = false;
      } else {
        game->cur_menu = MENU_SAVE;
        game->paused = true;
      }
    }
  }

  for (int i = 0; i < game->world.beings_amount; i++) {
    being_tick(&game->world.beings[i]);
  }

  handle_tile_interaction(game);

  handle_item_pickup(game);

  game->sound_manager.sound_timer += GetFrameTime();

#ifdef SURTUR_DEBUG
  debug_tick();
#endif
}

// RENDERING

void game_render(Game *game) {
  Vec2f mouse_pos = GetMousePosition();
  Vec2f mouse_world_pos = GetScreenToWorld2D(mouse_pos, game->player.cam);

  world_render_layer(&game->world, TILE_LAYER_GROUND);

  world_render_layer_top_split(&game->world, &game->player, true);

  for (int i = 0; i < game->world.beings_amount; i++) {
    being_render(&game->world.beings[i]);
  }

  game_render_particles(game, true);

  player_render(&game->player);

  world_render_layer_top_split(&game->world, &game->player, false);

  int x_index = floor_div(mouse_world_pos.x, TILE_SIZE);
  int y_index = floor_div(mouse_world_pos.y, TILE_SIZE);
  Rectangle rec =
      (Rectangle){.x = x_index * (TILE_SIZE), .y = y_index * (TILE_SIZE), .width = (TILE_SIZE), .height = (TILE_SIZE)};
  bool slot_selected = game_slot_selected();
  bool interaction_in_range =
      abs((int)game->player.box.x - x_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE &&
      abs((int)game->player.box.y - y_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE;

  if (!slot_selected && interaction_in_range) {
    rec_draw_outline(rec, BLUE);
  }
}

void game_render_overlay(Game *game) {
  Vec2i pos = vec2i(SCREEN_WIDTH - (3.5 * 16) - 30, (SCREEN_HEIGHT / 2.0f) - (3.5 * 8));
  DrawTextureEx(MAIN_HAND_SLOT_TEXTURE, (Vector2){pos.x, pos.y}, 0, 4.5, WHITE);
  item_render(&game->player.held_item, pos.x + 2 * 3.5, pos.y + 2 * 3.5);

#ifdef SURTUR_DEBUG
  tile_render_scaled(&game->debug_options.selected_tile_to_place_instance, 4);
  debug_render();
#endif
}

// UI/MENUS

static bool game_slot_selected() {
  Rectangle slot_rect = {.x = SCREEN_WIDTH - (3.5 * 16) - 30,
                         .y = (SCREEN_HEIGHT / 2.0f) - (3.5 * 8),
                         .width = 20 * 3.5,
                         .height = 20 * 3.5};
  return CheckCollisionPointRec(GetMousePosition(), slot_rect);
}

bool game_cur_menu_hides_game(Game *game) {
  return game->cur_menu == MENU_START || game->cur_menu == MENU_NEW_SAVE || game->cur_menu == MENU_LOAD_SAVE;
}

static void game_init_menu(Game *game) {
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

  UnloadSound(PLACE_SOUND);
  for (int i = 0; i < SOUND_BUFFER_LIMIT; i++) {
    UnloadSoundAlias(game->sound_manager.sound_buffer[i]);
  }
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
