#pragma once

#include "shared.h"
#include <raylib.h>
#include <stdlib.h>
#include "game.h"

typedef enum {
  UI_TOP,
  UI_BOTTOM,
  UI_LEFT,
  UI_RIGHT,
  UI_CENTER,
} UiPosition;

typedef enum {
  UI_HORIZONTAL,
  UI_VERTICAL,
} UiAlignment;

typedef struct {
  UiPosition positions[2];
  UiAlignment alignment;
  int padding;
} UiStyle;

typedef struct {
  int cur_x;
  int cur_y;
  int ui_height;
  UiStyle cur_style;
  bool simulate;
  Game *game;
  struct {
    int screen_width;
    int screen_height;
  } context;
} UiRenderer;

typedef void (*ButtonClickFunction)(UiRenderer *, Game *);

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

void ui_setup(UiRenderer *renderer);

void ui_set_style(UiRenderer *renderer, UiStyle style);

void ui_button_render_ex(UiRenderer *renderer, ButtonUiComponent component);

void ui_button_render_offset(UiRenderer *renderer, const char *text, Texture2D texture,
                      Texture2D selected_texture,
                      ButtonClickFunction on_click_func, Vec2i offset);

void ui_button_render(UiRenderer *renderer, const char *text, Texture2D texture,
                      Texture2D selected_texture,
                      ButtonClickFunction on_click_func);

void ui_text_render_ex(UiRenderer *renderer, TextUiComponent component);

void ui_text_render(UiRenderer *renderer, const char *text);

void ui_spacing_render_ex(UiRenderer *renderer, SpacingUiComponent component);

void ui_spacing_render(UiRenderer *renderer, int spacing_height);


void ui_container_create(UiRenderer *renderer);

void ui_container_destroy(UiRenderer *renderer);