#pragma once

#include "shared.h"
#include <stdlib.h>

typedef void (*ButtonClickFunction)(void *, void *);

typedef struct {
  Texture2D texture;
  const char *message;
  Vec2i dimensions;
  Vector2 offset;
  ButtonClickFunction on_click_func;
} ButtonComponent;

typedef struct {
  Vec2i dimensions;
  Vector2 offset;
} SpacingComponent;

typedef struct {
  const char *message;
  Vec2i dimensions;
  Vector2 offset;
  Color color;
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
} UiComponent;

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
  int padding;
} UiStyle;

typedef struct {
  UiComponent components[64];
  size_t components_amount;
  UiStyle style;
} Layout;

bool style_check_valid(const UiStyle *style);

void layout_set_style(Layout *layout, UiStyle style);

void layout_text(Layout *layout, TextComponent text);

void layout_text_simple(Layout *layout, const char *message);

void layout_button(Layout *layout, ButtonComponent button);

void layout_button_simple(Layout *layout, const char *message, ButtonClickFunction on_click_func);

void layout_spacing(Layout *layout, SpacingComponent spacing);

void layout_spacing_simple(Layout *layout, int spacing);
