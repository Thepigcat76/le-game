#include "../include/ui.h"
#include "../include/config.h"
#include "raylib.h"

static void ui_component_text_render(Renderer *renderer,
                                     const TextComponent *component,
                                     RenderContext ctx) {
  renderer->cur_x = (ctx.screen_width - component->dimensions.x) / 2;
  DrawText(component->message, renderer->cur_x, renderer->cur_y,
           CONFIG.default_font_size, component->color);
  // TODO: Move cur coordinate by dimension depdening on style
  // renderer->cur_x += component->dimensions.x + component->offset.x;
  renderer->cur_y +=
      component->dimensions.y + component->offset.y + ctx.style.padding;
}

static void ui_component_spacing_render(Renderer *renderer,
                                        const SpacingComponent *component,
                                        RenderContext ctx) {}

static void ui_component_button_render(Renderer *renderer,
                                       const ButtonComponent *component,
                                       RenderContext ctx) {}

void ui_component_render(Renderer *renderer, const UiComponent *component,
                         RenderContext ctx) {
  switch (component->type) {
  case COMPONENT_TEXT:
    ui_component_text_render(renderer, &component->var.text, ctx);
    break;
  case COMPONENT_SPACING:
    ui_component_spacing_render(renderer, &component->var.spacing, ctx);
    break;
  case COMPONENT_BUTTON:
    ui_component_button_render(renderer, &component->var.button, ctx);
    break;
  }
}

static int ui_component_height(const UiComponent *component) {
  switch (component->type) {
  case COMPONENT_TEXT:
    return component->var.text.dimensions.y;
  case COMPONENT_SPACING:
    return component->var.spacing.dimensions.y;
  case COMPONENT_BUTTON:
    return component->var.button.dimensions.y;
  }
}

void ui_layout_render(const Layout *layout, RenderContext ctx) {
  Renderer renderer = {.cur_x = 0, .cur_y = 0};
  int height = 0;
  for (int i = 0; i < layout->components_amount; i++) {
    height += ui_component_height(&layout->components[i]);
    if (layout->components[i].type != COMPONENT_SPACING) {
      height += layout->style.padding;
    }
  }
  if (layout->components[layout->components_amount - 1].type !=
      COMPONENT_SPACING) {
    height -= layout->style.padding;
  }
  ctx.total_height = height;
  ctx.start_x = ctx.screen_width / 2;
  ctx.start_y = (ctx.screen_height - ctx.total_height) / 2;
  ctx.style = layout->style;
  TraceLog(LOG_INFO, "Height: %d, start y: %d", height, ctx.start_y);

  renderer.cur_x = ctx.start_x;
  renderer.cur_y = ctx.start_y;

  for (int i = 0; i < layout->components_amount; i++) {
    ui_component_render(&renderer, &layout->components[i], ctx);
  }
}
