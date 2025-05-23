#include "../include/shared.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

Texture2D BUTTON_TEXTURE;
Texture2D BUTTON_SELECTED_TEXTURE;

Texture2D BACK_TO_GAME_BUTTON_TEXTURE;
Texture2D VISUAL_SETTINGS_BUTTON_TEXTURE;
Texture2D GAME_SETTINGS_BUTTON_TEXTURE;
Texture2D LEAVE_GAME_BUTTON_TEXTURE;

Texture2D BACK_TO_GAME_SELECTED_BUTTON_TEXTURE;
Texture2D VISUAL_SETTINGS_SELECTED_BUTTON_TEXTURE;
Texture2D GAME_SETTINGS_SELECTED_BUTTON_TEXTURE;
Texture2D LEAVE_GAME_SELECTED_BUTTON_TEXTURE;

Texture2D BACKPACK_BACK_GROUND;

int TILE_ANIMATION_FRAMES[TILE_TYPE_AMOUNT];

char *read_file_to_string(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    perror("Error opening file");
    return NULL;
  }

  // Seek to the end of the file to get the file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET); // Go back to the beginning of the file

  if (file_size < 0) {
    perror("Error determining file size");
    fclose(file);
    return NULL;
  }

  // Allocate memory for the file content + null terminator
  char *buffer = (char *)malloc(file_size + 1);
  if (buffer == NULL) {
    perror("Error allocating memory");
    fclose(file);
    return NULL;
  }

  // Read the file content into the buffer
  size_t read_size = fread(buffer, 1, file_size, file);
  if (read_size != file_size) {
    perror("Error reading file");
    free(buffer);
    fclose(file);
    return NULL;
  }

  // Null-terminate the string
  buffer[file_size] = '\0';

  fclose(file);
  return buffer;
}

void shared_init() {
  BUTTON_TEXTURE = load_texture("res/assets/gui/button.png");
  BUTTON_SELECTED_TEXTURE = load_texture("res/assets/gui/button_selected.png");

  BACK_TO_GAME_BUTTON_TEXTURE = load_texture("res/assets/gui/back_to_game_button.png");
  VISUAL_SETTINGS_BUTTON_TEXTURE = load_texture("res/assets/gui/visual_settings_button.png");
  GAME_SETTINGS_BUTTON_TEXTURE = load_texture("res/assets/gui/game_settings_button.png");
  LEAVE_GAME_BUTTON_TEXTURE = load_texture("res/assets/gui/leave_game_button.png");

  BACK_TO_GAME_SELECTED_BUTTON_TEXTURE = load_texture("res/assets/gui/back_to_game_button_selected.png");
  VISUAL_SETTINGS_SELECTED_BUTTON_TEXTURE = load_texture("res/assets/gui/visual_settings_button_selected.png");
  GAME_SETTINGS_SELECTED_BUTTON_TEXTURE = load_texture("res/assets/gui/game_settings_button_selected.png");
  LEAVE_GAME_SELECTED_BUTTON_TEXTURE = load_texture("res/assets/gui/leave_game_button_selected.png");

  BACKPACK_BACK_GROUND = load_texture("res/assets/gui/backpack_slots.png");
}

Vec2i vec2i(int x, int y) { return (Vec2i){.x = x, .y = y}; }

Vec2f vec2f(float x, float y) { return (Vec2f){.x = x, .y = y}; }

Vec2i vec2i_sub(Vec2i vec0, Vec2i vec1) { return vec2i(vec0.x - vec1.x, vec0.y - vec1.y); }

Vec2i vec2i_add(Vec2i vec0, Vec2i vec1) { return vec2i(vec0.x + vec1.x, vec0.y + vec1.y); }

void rec_draw_outline(Rectangle rec, Color color) { DrawRectangleLinesEx(rec, 1, color); }

bool vec2_eq(const Vec2i *vec1, const Vec2i *vec2) { return vec1->x == vec2->x && vec1->y == vec2->y; }

Texture2D load_texture(char *path) {
  Image image = LoadImage(path);
  Texture2D texture = LoadTextureFromImage(image);
  UnloadImage(image);
  return texture;
}

Rectf rectf(float x, float y, float w, float h) { return (Rectf){.x = x, .y = y, .width = w, .height = h}; }

void DrawTextureRecEx(Texture2D texture, Rectangle source, Vector2 pos, float rotation, float scale, Color tint) {
  DrawTexturePro(texture, source,
                 (Rectangle){.x = pos.x, .y = pos.y, .width = source.width * scale, .height = source.height * scale},
                 (Vector2){.x = source.width * scale / 2, .y = source.height * scale / 2}, rotation, tint);
}

int floor_div(int a, int b) { return (a >= 0) ? (a / b) : ((a - b + 1) / b); }

int floor_mod(int a, int b) {
  int r = a % b;
  return (r < 0) ? r + b : r;
}
