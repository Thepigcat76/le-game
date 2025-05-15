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
}

Vec2i vec2i(int x, int y) { return (Vec2i){.x = x, .y = y}; }

bool vec2_eq(const Vec2i *vec1, const Vec2i *vec2) {
  return vec1->x == vec2->x && vec1->y == vec2->y;
}

Texture2D load_texture(char *path) {
  Image image = LoadImage(path);
  Texture2D texture = LoadTextureFromImage(image);
  UnloadImage(image);
  return texture;
}