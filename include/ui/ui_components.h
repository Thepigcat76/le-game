#pragma once

#include "raylib.h"
#include "ui_style.h"
#include "../textures.h"
#include "../item.h"
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

#define OPT_TEX(tex) (struct _optional_texture){.texture_handle = tex, .present = true}

struct _optional_texture {
  TextureHandle texture_handle;
  bool present;
};

typedef struct {
  struct _optional_texture texture;
  struct _optional_texture selected_texture;
  const char *message;
  i32 width;
  i32 height;
  i32 x_offset;
  i32 y_offset;
  f32 text_x_offset;
  f32 text_y_offset;
  ButtonClickFunction on_click_func;
} ButtonUiComponent;

typedef struct {
  i32 width;
  i32 height;
  i32 x_offset;
  i32 y_offset;
} SpacingUiComponent;

typedef struct {
  Texture2D texture;
} TextureUiComponent;

typedef struct {
  Texture2D texture;
  i32 width;
  i32 height;
  i32 x_offset;
  i32 y_offset;
} BackgroundUiComponent;

typedef struct {
  const char *text;
  i32 x_offset;
  i32 y_offset;
  Color color;
} TextUiComponent;

typedef struct {
  char *buf;
  size_t len;
  size_t max_len;
} TextInputBuffer;

typedef struct {
  struct _optional_texture texture;
  TextInputBuffer *text_input;
  bool *selected;
  i32 width;
  i32 height;
  i32 x_offset;
  i32 y_offset;
  f32 text_x_offset;
  f32 text_y_offset;
  Color color;
} TextInputUiComponent;

typedef struct {
  i32 width;
  i32 height;
  i32 x_offset;
  i32 y_offset;
  bool has_scrollbar;
  i32 *scroll_y_offset;
  UiStyle group_style;
} GroupUiComponent;

typedef struct {
  i32 width;
  i32 height;
  i32 x_offset;
  i32 y_offset;
  bool fake;
  ItemInstance *item;
} SlotUiComponent;
