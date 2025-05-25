#pragma once

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
  float scale;
  float font_scale;
} UiStyle;
