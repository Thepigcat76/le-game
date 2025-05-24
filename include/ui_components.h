#pragma once

#include "raylib.h"
#include <stdlib.h>

typedef void (*ButtonClickFunction)();

typedef struct {
  Texture2D texture;
  Texture2D selected_texture;
  const char *message;
  int width;
  int height;
  int x_offset;
  int y_offset;
  float text_x_offset;
  float text_y_offset;
  ButtonClickFunction on_click_func;
} ButtonUiComponent;

typedef struct {
  int width;
  int height;
  int x_offset;
  int y_offset;
} SpacingUiComponent;

typedef struct {
  const char *text;
  int width;
  int height;
  int x_offset;
  int y_offset;
  Color color;
} TextUiComponent;

typedef struct {
  char *buf;
  size_t len;
  size_t max_len;
} TextInputBuffer;

typedef struct {
  Texture2D texture;
  TextInputBuffer *text_input;
  int width;
  int height;
  int x_offset;
  int y_offset;
  Color color;
} TextInputUiComponent;
