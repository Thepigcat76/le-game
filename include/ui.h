#pragma once

#include "assets.h"
#include "shared.h"
#include "ui/ui_components.h"
#include "ui/ui_style.h"
#include <lilc/alloc.h>
#include <raylib.h>
#include <stdlib.h>

extern Texture2D BUTTON_TEXTURE_DEFAULT;
extern Texture2D BUTTON_TEXTURE_SELECTED_DEFAULT;

typedef struct {
  GroupUiComponent component;
  i32 prev_x;
  i32 prev_y;
} UiGroup;

typedef struct {
  i32 screen_width;
  i32 screen_height;
} UiContext;

typedef struct _ui_renderer {
  i32 cur_x;
  i32 cur_y;
  i32 ui_width;
  i32 ui_height;
  bool simulate;
  UiStyle cur_style;
  UiStyle initial_style;
  UiGroup groups[MAX_UI_GROUPS_AMOUNT];
  size_t groups_amount;
  UiContext context;

  AssetManager *asset_manager;

  Bump ui_bump;
  Allocator ui_bump_allocator;
} UiRenderer;

#ifdef CTX_SERVER
extern UiRenderer UI_RENDERER;
#define UI_RENDERER_PTR (&UI_RENDERER)
#else
#define UI_RENDERER_PTR (&CLIENT_GAME.ui_renderer)
#endif

void ui_renderer_init(UiRenderer *renderer);

// Setup (Order of declaratiion should also be)

// Needs to be called after setting the style
#define UI_SETUP(...) ui_setup(UI_RENDERER_PTR, (UiStyle)__VA_ARGS__)

#define UI_BACKGROUND(...) ui_set_background(UI_RENDERER_PTR, (BackgroundUiComponent)__VA_ARGS__)

void ui_setup(UiRenderer *renderer, UiStyle style);

void ui_set_background(UiRenderer *renderer, BackgroundUiComponent component);

float ui_scale(UiRenderer *renderer);

// COMPONENTS

#define RENDER_BUTTON(...) ui_button_render(UI_RENDERER_PTR, (ButtonUiComponent)__VA_ARGS__)

#define RENDER_TEXT(...) ui_text_render(UI_RENDERER_PTR, (TextUiComponent)__VA_ARGS__)

#define RENDER_TEXT_INPUT(...) ui_text_input_render(UI_RENDERER_PTR, (TextInputUiComponent)__VA_ARGS__)

#define RENDER_TEXTURE(...) ui_texture_render(UI_RENDERER_PTR, (TextureUiComponent)__VA_ARGS__)

#define RENDER_SPACING(...) ui_spacing_render(UI_RENDERER_PTR, (SpacingUiComponent)__VA_ARGS__)

#define RENDER_SLOT(...) ui_slot_render(UI_RENDERER_PTR, (SlotUiComponent)__VA_ARGS__)

#define UI_GROUP_CREATE(...) ui_group_create(UI_RENDERER_PTR, (GroupUiComponent)__VA_ARGS__)

#define UI_GROUP_DESTROY() ui_group_destroy(UI_RENDERER_PTR)

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

// SLOT

void ui_slot_render(UiRenderer *renderer, SlotUiComponent component);

// GROUP

void ui_group_create(UiRenderer *renderer, GroupUiComponent component);

void ui_group_destroy(UiRenderer *renderer);
