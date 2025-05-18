#pragma once

#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CHUNK_SIZE 16

#define WORLD_LOADED_CHUNKS 9

#define TILE_SIZE 16

#define STR_LIT_LEN(str_literal) sizeof(str_literal) - 1

#define ASSETS_DIR "res/assets/"

#define PATH_MAX 4096

extern Texture2D BUTTON_TEXTURE;
extern Texture2D BUTTON_SELECTED_TEXTURE;

extern Texture2D BACK_TO_GAME_BUTTON_TEXTURE;
extern Texture2D VISUAL_SETTINGS_BUTTON_TEXTURE;
extern Texture2D GAME_SETTINGS_BUTTON_TEXTURE;
extern Texture2D LEAVE_GAME_BUTTON_TEXTURE;

char *read_file_to_string(const char *filename);

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

typedef Vec2i ChunkPos;
typedef Vec2i TilePos;

void shared_init();

Vec2i vec2i(int x, int y);

bool vec2_eq(const Vec2i *vec1, const Vec2i *vec2);

Texture2D load_texture(char *path);

int shared_random(int min, int max);

Rectangle rect(float x, float y, float w, float h);