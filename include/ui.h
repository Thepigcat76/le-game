#pragma once

#include "shared.h"
#include "ui/ui_components.h"
#include "ui/ui_style.h"
#include <raylib.h>
#include <stdlib.h>

typedef struct {
  int cur_x;
  int cur_y;
  int ui_height;
  UiStyle cur_style;
  bool simulate;
  struct {
    int screen_width;
    int screen_height;
  } context;
} UiRenderer;

// Setup (Order of declaratiion should also be)

void ui_set_style(UiRenderer *renderer, UiStyle style);

void ui_setup(UiRenderer *renderer);

void ui_set_background(UiRenderer *renderer, Texture2D texture);

float ui_scale(UiRenderer *renderer);

// COMPONENTS

// BUTTONS

void ui_button_render_ex(UiRenderer *renderer, ButtonUiComponent component);

void ui_button_render_dimensions_offset(UiRenderer *renderer, const char *text, Texture2D texture,
                                        Texture2D selected_texture, ButtonClickFunction on_click_func, Vec2i offset,
                                        Vec2i dimensions);

void ui_button_render_offset(UiRenderer *renderer, const char *text, Texture2D texture, Texture2D selected_texture,
                             ButtonClickFunction on_click_func, Vec2i offset);

void ui_button_render(UiRenderer *renderer, const char *text, Texture2D texture, Texture2D selected_texture,
                      ButtonClickFunction on_click_func);

// TEXT

void ui_text_render_ex(UiRenderer *renderer, TextUiComponent component);

void ui_text_render_offset(UiRenderer *renderer, const char *text, Vec2i offset);

void ui_text_render(UiRenderer *renderer, const char *text);

// TEXT INPUT

void ui_text_input_render_ex(UiRenderer *renderer, TextInputUiComponent component);

void ui_text_input_render(UiRenderer *renderer, Texture2D texture, TextInputBuffer *text_input_buf);

void ui_text_input_render_dimensions(UiRenderer *renderer, Texture2D texture, TextInputBuffer *text_input_buf,
                                     Vec2i dimensions);

// SPACING

void ui_spacing_render_ex(UiRenderer *renderer, SpacingUiComponent component);

void ui_spacing_render(UiRenderer *renderer, int spacing_height);

// TEXTURE

void ui_texture_render(UiRenderer *renderer, Texture2D texture);

// CONTAINER

void ui_group_create(UiRenderer *renderer);

void ui_group_destroy(UiRenderer *renderer);
