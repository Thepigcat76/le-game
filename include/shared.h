#pragma once

#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CHUNK_SIZE 16

#define WORLD_LOADED_CHUNKS 40

#define TILE_SIZE 16

#define STR_LIT_LEN(str_literal) sizeof(str_literal) - 1

#define ASSETS_DIR "res/assets/"

#define PATH_MAX 4096

#define TILES_AMOUNT 7

#define SAVE_DATA_BYTES 10000

#define MAX_ENTITIES_AMOUNT 100

#define MAX_UI_TEXTURES_AMOUNT 100

#define TILE_LAYERS_AMOUNT 2

#define MAX_PARTICLES_AMOUNT 500

extern Texture2D BUTTON_TEXTURE;
extern Texture2D BUTTON_SELECTED_TEXTURE;

extern Texture2D BACK_TO_GAME_BUTTON_TEXTURE;
extern Texture2D VISUAL_SETTINGS_BUTTON_TEXTURE;
extern Texture2D GAME_SETTINGS_BUTTON_TEXTURE;
extern Texture2D LEAVE_GAME_BUTTON_TEXTURE;

extern Texture2D BACK_TO_GAME_SELECTED_BUTTON_TEXTURE;
extern Texture2D VISUAL_SETTINGS_SELECTED_BUTTON_TEXTURE;
extern Texture2D GAME_SETTINGS_SELECTED_BUTTON_TEXTURE;
extern Texture2D LEAVE_GAME_SELECTED_BUTTON_TEXTURE;

extern Texture2D BACKPACK_BACK_GROUND;

extern int TILE_ANIMATION_FRAMES[TILES_AMOUNT];

char *read_file_to_string(const char *filename);

#define DIRECTIONS_AMOUNT 4

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

void rec_draw_outline(Rectangle rec, Color color);

Vec2i vec2i(int x, int y);

bool vec2_eq(const Vec2i *vec1, const Vec2i *vec2);

Texture2D load_texture(char *path);

Rectangle rect(float x, float y, float w, float h);

void DrawTextureRecEx(Texture2D texture, Rectangle source, Vector2 pos, float rotation, float scale, Color tint);

int floor_div(int a, int b);

int floor_mod(int a, int b);
