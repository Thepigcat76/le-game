#pragma once

#include "raylib.h"
#include "ui_style.h"
#include <stdlib.h>

typedef struct {
  enum { BUTTON_CLICK_FUNC_NO_ARGS, BUTTON_CLICK_FUNC_WITH_ARGS } func_type;
  union {
    void (*on_click_no_args)();
    void (*on_click_with_args)(void *);
  } func_var;
  void *args;
} ButtonClickFunction;

ButtonClickFunction button_click_simple(void (*on_click_func)());

ButtonClickFunction button_click_args(void (*on_click_func)(void *), void *args);

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
  bool *selected;
  int width;
  int height;
  int x_offset;
  int y_offset;
  float text_x_offset;
  float text_y_offset;
  Color color;
} TextInputUiComponent;

typedef struct {
  int width;
  int height;
  int x_offset;
  int y_offset;
  bool has_scrollbar;
  int *scroll_y_offset;
  UiStyle group_style;
} GroupUiComponent;
