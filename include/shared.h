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

#define SAVE_DATA_BYTES 20000

#define MAX_WORLD_BEINGS_AMOUNT 100

#define MAX_UI_TEXTURES_AMOUNT 100

#define MAX_BEING_MEMORIES_AMOUNT 100

#define MAX_BEING_ACTIVITIES_AMOUNT 100

#define MAX_PARTICLES_AMOUNT 500

#define TILE_LAYERS_AMOUNT 2

#define TILE_CATEGORIES_AMOUNT 8

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

void create_dir(const char *dir_name);

char *read_file_to_string(const char *filename);

bool string_starts_with(const char *str, const char *prefix);

// RETURNS HOW OFTEN THE CHARACTER IS CONTAINED
int string_contains(const char *string, char c);

bool is_dir(const char *path);

void rec_draw_outline(Rectf rec, Color color);

Vec2i vec2i(int x, int y);

Vec2i vec2i_add(Vec2i vec0, Vec2i vec1);

Vec2i vec2i_sub(Vec2i vec0, Vec2i vec1);

Vec2f vec2f(float x, float y);

bool vec2_eq(const Vec2i *vec1, const Vec2i *vec2);

Dimensionsf dimf(float width, float height);

Rectf rectf_from_dimf(float x, float y, Dimensionsf dimf);

Rectf rectf(float x, float y, float w, float h);

Color color_rgb(int r, int g, int b);

Color color_rgba(int r, int g, int b, int a);

void DrawTextureRecEx(Texture2D texture, Rectf source, Vector2 pos, float rotation, float scale, Color tint);

int floor_div(int a, int b);

int floor_mod(int a, int b);

int signum(int s);

Direction direction_from_delta(int x, int y);

float string_to_world_seed(const char *str);
