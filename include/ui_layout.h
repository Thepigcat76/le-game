#pragma once

#include "shared.h"
#include <stdlib.h>

typedef void (*ButtonClickFunction)(struct _game *, struct _menu *);

typedef struct {
  Texture2D texture;
  const char *message;
  Vec2i dimensions;
  Vector2 position;
  ButtonClickFunction on_click_func;
} ButtonComponent;

typedef struct {
  Vec2i dimensions;
  Vector2 position;
} SpacingComponent;

typedef struct {
  const char *message;
  Vec2i dimensions;
  Vector2 position;
} TextComponent;

typedef struct {
  enum {
    COMPONENT_TEXT,
    COMPONENT_SPACING,
    COMPONENT_BUTTON,
  } type;
  union {
    TextComponent text;
    SpacingComponent spacing;
    ButtonComponent button;
  } var;
} GuiComponent;

typedef struct {
  GuiComponent components[64];
  size_t components_amount;
} Layout;

void layout_text(Layout *layout, TextComponent text);

void layout_button(Layout *layout, ButtonComponent button);

void layout_spacing(Layout *layout, SpacingComponent spacing);
