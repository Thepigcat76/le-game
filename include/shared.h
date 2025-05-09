#pragma once

#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CHUNK_SIZE 16

#define WORLD_LOADED_CHUNKS 9

#define TILE_SIZE 16

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

Vec2i vec2i(int x, int y);

bool vec2_eq(const Vec2i *vec1, const Vec2i *vec2);

Texture2D load_texture(char *path);
