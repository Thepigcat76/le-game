#pragma once

#include "shared.h"
#include "ui/ui_components.h"
#include "ui/ui_style.h"
#include <raylib.h>
#include <stdlib.h>

typedef struct {
  GroupUiComponent component;
  int prev_x;
  int prev_y;
} UiGroup;

typedef struct {
  int cur_x;
  int cur_y;
  int ui_height;
  bool simulate;
  UiStyle cur_style;
  UiStyle initial_style;
  UiGroup groups[MAX_UI_GROUPS_AMOUNT];
  size_t groups_amount;
  struct {
    int screen_width;
    int screen_height;
  } context;
} UiRenderer;

// Setup (Order of declaratiion should also be)

// Needs to be called after setting the style
#define UI_SETUP(...) ui_setup(&CLIENT_GAME.ui_renderer, (UiStyle)__VA_ARGS__)

#define UI_BACKGROUND(...) ui_set_background(&CLIENT_GAME.ui_renderer, (BackgroundUiComponent)__VA_ARGS__)

void ui_setup(UiRenderer *renderer, UiStyle style);

void ui_set_background(UiRenderer *renderer, BackgroundUiComponent component);

float ui_scale(UiRenderer *renderer);

// COMPONENTS

#define RENDER_BUTTON(...) ui_button_render(&CLIENT_GAME.ui_renderer, (ButtonUiComponent)__VA_ARGS__)

#define RENDER_TEXT(...) ui_text_render(&CLIENT_GAME.ui_renderer, (TextUiComponent)__VA_ARGS__)

#define RENDER_TEXT_INPUT(...) ui_text_input_render(&CLIENT_GAME.ui_renderer, (TextInputUiComponent)__VA_ARGS__)

#define RENDER_TEXTURE(...) ui_texture_render(&CLIENT_GAME.ui_renderer, (TextureUiComponent)__VA_ARGS__)

#define RENDER_SPACING(...) ui_spacing_render(&CLIENT_GAME.ui_renderer, (SpacingUiComponent)__VA_ARGS__)

#define UI_GROUP_CREATE(...) ui_group_create(&CLIENT_GAME.ui_renderer, (GroupUiComponent)__VA_ARGS__)

#define UI_GROUP_DESTROY() ui_group_destroy(&CLIENT_GAME.ui_renderer)

// BUTTONS

void ui_button_render(UiRenderer *renderer, ButtonUiComponent component);

// TEXT

void ui_text_render(UiRenderer *renderer, TextUiComponent component);

// TEXT INPUT

void ui_text_input_render(UiRenderer *renderer, TextInputUiComponent component);

// SPACING

void ui_spacing_render(UiRenderer *renderer, SpacingUiComponent component);

// TEXTURE

void ui_texture_render(UiRenderer *renderer, TextureUiComponent component);

// GROUP

void ui_group_create(UiRenderer *renderer, GroupUiComponent component);

void ui_group_destroy(UiRenderer *renderer);
