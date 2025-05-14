#pragma once

#include "ui_layout.h"

typedef struct {
  int cur_x;
  int cur_y;
} Renderer;

typedef struct {
  int screen_width;
  int screen_height;
  int total_height;
  int start_x;
  int start_y;
  UiStyle style;
} RenderContext;

void ui_layout_render(const Layout *layout, RenderContext ctx);

void ui_component_render(Renderer *renderer, const UiComponent *component, RenderContext ctx);
