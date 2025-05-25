#pragma once

#include <raylib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define CHUNK_SIZE 16

#define WORLD_LOADED_CHUNKS 40

#define TILE_SIZE 16

#define STR_LIT_LEN(str_literal) sizeof(str_literal) - 1

#define ASSETS_DIR "res/assets/"

#define SAVE_DIR "save/"

#define PATH_MAX 4096

#define SAVE_DATA_BYTES 10000

#define MAX_ENTITIES_AMOUNT 100

#define MAX_UI_TEXTURES_AMOUNT 100

#define TILE_LAYERS_AMOUNT 2

#define MAX_PARTICLES_AMOUNT 500

#define TILE_TYPE_AMOUNT 8

#define ITEM_TYPE_AMOUNT 9

extern Texture2D DEBUG_BUTTON_TEXTURE;
extern Texture2D DEBUG_BUTTON_SELECTED_TEXTURE;
extern Texture2D TEXT_INPUT_TEXTURE;

extern Texture2D BUTTON_TEXTURE;
extern Texture2D BUTTON_SELECTED_TEXTURE;

extern Texture2D BACKPACK_BACK_GROUND;

extern Texture2D MAIN_HAND_SLOT_TEXTURE;
extern Texture2D OFF_HAND_SLOT_TEXTURE;

extern Texture2D SAVE_SLOT_TEXTURE;

extern int TILE_ANIMATION_FRAMES[TILE_TYPE_AMOUNT];

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

// RAYLIB Typedefs
typedef Vector2 Vec2f;
typedef Rectangle Rectf;

typedef Vec2i ChunkPos;
typedef Vec2i TilePos;

void shared_init();

void create_dir(const char *dir_name);

char *read_file_to_string(const char *filename);

bool string_starts_with(const char *str, const char *prefix);

bool is_dir(const char *path);

void rec_draw_outline(Rectf rec, Color color);

Vec2i vec2i(int x, int y);

Vec2i vec2i_add(Vec2i vec0, Vec2i vec1);

Vec2i vec2i_sub(Vec2i vec0, Vec2i vec1);

Vec2f vec2f(float x, float y);

bool vec2_eq(const Vec2i *vec1, const Vec2i *vec2);

Texture2D load_texture(char *path);

Rectf rectf(float x, float y, float w, float h);

void DrawTextureRecEx(Texture2D texture, Rectf source, Vector2 pos, float rotation, float scale, Color tint);

int floor_div(int a, int b);

int floor_mod(int a, int b);

float string_to_world_seed(const char *str);
