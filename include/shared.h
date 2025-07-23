#pragma once

#include <dirent.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Only project include
#include "../include/alloc.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CHUNK_SIZE 16

#define WORLD_LOADED_CHUNKS 10

#define TILE_SIZE 16

#define STR_LIT_LEN(str_literal) sizeof(str_literal) - 1

#define ASSETS_DIR "res/assets/"

#define SAVE_DIR "save/"

#define PATH_MAX 4096

#define SAVE_DATA_BYTES 20000

#define MAX_WORLD_BEINGS_AMOUNT 100

#define MAX_UI_TEXTURES_AMOUNT 100

#define MAX_BEING_MEMORIES_AMOUNT 100

#define MAX_BEING_ACTIVITIES_AMOUNT 100

#define MAX_PARTICLES_AMOUNT 500

#define TILE_LAYERS_AMOUNT 2

#define MAX_CATEGORIES_AMOUNT 256

#define MAX_TILE_TYPES 256

#define MAX_ITEM_TYPES 256

#define BEINGS_AMOUNT 2

#define SOUND_BUFFER_LIMIT 64

#define SOUND_COOLDOWN 0.125f

#define DIRECTIONS_AMOUNT 4

#define MAX_UI_GROUPS_AMOUNT 64

#define RECOMMENDED_LOADED_CHUNKS 25

#define MAX_SAVE_NAMES_AMOUNT 100

#define MAX_GAME_FEATURES_AMOUNT 1000

#define DEFAULT_ITEM_BEING_ITEM ITEM_GRASS

#define GLOBAL_BUMP_CAPACITY 256

#define ARRAY_INITIAL_CAPACITY 16

#define MAX_PLAYERS 256

#define TICK_RATE 20                     // ticks per second
#define TICK_INTERVAL (1.0f / TICK_RATE) // seconds per tick

extern Texture2D DEBUG_BUTTON_TEXTURE;
extern Texture2D DEBUG_BUTTON_SELECTED_TEXTURE;
extern Texture2D TEXT_INPUT_TEXTURE;

extern Texture2D BUTTON_TEXTURE;
extern Texture2D BUTTON_SELECTED_TEXTURE;

extern Texture2D BACKPACK_BACK_GROUND;

extern Texture2D MAIN_HAND_SLOT_TEXTURE;
extern Texture2D OFF_HAND_SLOT_TEXTURE;

extern Texture2D SAVE_SLOT_TEXTURE;
extern Texture2D SAVE_SLOT_SELECTED_TEXTURE;

extern Texture2D NPC_TEXTURES[DIRECTIONS_AMOUNT];
extern Texture2D NPC_ANIMATED_TEXTURES[DIRECTIONS_AMOUNT];

extern Texture2D NPC_TEXTURE_OLD_MAN;

typedef enum {
  DIRECTION_UP,
  DIRECTION_DOWN,
  DIRECTION_LEFT,
  DIRECTION_RIGHT,
} Direction;

typedef struct {
  int x;
  int y;
} Vec2i;

typedef struct {
  int width;
  int height;
} Dimensionsf;

// RAYLIB Typedefs
typedef Vector2 Vec2f;
typedef Rectangle Rectf;

typedef Vec2i ChunkPos;
typedef Vec2i TilePos;

void shared_init();

char *read_file_to_string(const char *filename);

bool string_starts_with(const char *str, const char *prefix);

// RETURNS HOW OFTEN THE CHARACTER IS CONTAINED
int string_contains(const char *string, char c);

// FILE SYSTEM - DIRECTORIES

bool dir_exists(const char *path);

void dir_create(const char *dir_name);

void rec_draw_outline(Rectf rec, Color color);

Vec2i vec2i(int x, int y);

Vec2i vec2i_add(Vec2i vec0, Vec2i vec1);

Vec2i vec2i_sub(Vec2i vec0, Vec2i vec1);

Vec2f vec2f(float x, float y);

bool vec2_eq(const Vec2i *vec1, const Vec2i *vec2);

bool vec2f_eq(Vec2f vec1, Vec2f vec2);

Dimensionsf dimf(float width, float height);

Rectf rectf_from_dimf(float x, float y, Dimensionsf dimf);

Rectf rectf(float x, float y, float w, float h);

Color color_rgb(int r, int g, int b);

Color color_rgba(int r, int g, int b, int a);

void DrawTextureRecEx(Texture2D texture, Rectf source, Vector2 pos, float rotation, float scale, Color tint);

int floor_div(int a, int b);

int floor_mod(int a, int b);

float lerpf(float a, float b, float t);

int signum(int s);

Direction direction_from_delta(int x, int y);

float string_to_world_seed(const char *str);

// ip_addr_buf needs to be at least 16 characters long + null terminator
// Return -1 if we encounter error
int ip_addr(char *ip_addr_buf);

void stack_trace_print(void);

Color color_from_str(const char *color_literal);

char *str_cpy(const char *in, Allocator *alloc);

char *str_cpy_heap(const char *in);

#define ANSI_RED "\033[1;31m"
#define ANSI_RESET "\033[0m"

#define ASSERT(cond, fmt, ...) do {\
  if (!(cond)) PANIC_FMT(fmt, __VA_ARGS__);\
} while (0)

#define PANIC(...)                                                                                                                         \
  do {                                                                                                                                     \
    puts(ANSI_RED "Program panicked" ANSI_RESET);                                                                                          \
    __VA_OPT__(puts(__VA_ARGS__);)                                                                                                         \
    puts("Crashed at:");                                                                                                                   \
    stack_trace_print();                                                                                                                   \
    exit(1);                                                                                                                               \
  } while (0)

#define PANIC_FMT(...)                                                                                                                \
  do {                                                                                                                                     \
    /* We allocate a huge buffer cuz this is gonna crash the program anyways lol */                                                        \
    char buf[4096 + 2];                                                                                                                    \
    __VA_OPT__(snprintf(buf, 4096 + 2, __VA_ARGS__);)                                                                               \
    buf[4096] = '\n';                                                                                                                      \
    buf[4096 + 1] = '\0';                                                                                                                  \
    PANIC(buf);                                                                                                                            \
  } while (0)

#define DIR_ITER(dir_name, entry, ...)                                                                                                     \
  do {                                                                                                                                     \
    DIR *_dir_ptr = opendir(dir_name);                                                                                                     \
    if (_dir_ptr == NULL) {                                                                                                                \
      PANIC_FMT("Failed to open %s", dir_name);                                                                                            \
    }                                                                                                                                      \
    struct dirent *entry;                                                                                                                  \
    while ((entry = readdir(_dir_ptr)) != NULL) {                                                                                          \
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {                                                           \
        continue;                                                                                                                          \
      }                                                                                                                                    \
      __VA_ARGS__                                                                                                                          \
    }                                                                                                                                      \
    closedir(_dir_ptr);                                                                                                                    \
  } while (0)

// The assignment consumes the key
#define IS_KEY_PRESSED(id) GAME.client_game->pressed_keys.id##_key_pressed && !(GAME.client_game->pressed_keys.id##_key_pressed = false)
#define IS_KEY_DOWN(id) GAME.client_game->pressed_keys.id##_key_down && !(GAME.client_game->pressed_keys.id##_key_down = false)

char *btos(bool b);

bool str_eq(const char *a, const char *b);
