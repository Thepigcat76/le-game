#include "../include/ui_layout.h"
#include <raylib.h>

static bool pos_valid(UiPosition pos0, UiPosition pos1) {
  return !((pos0 == UI_LEFT && pos1 == UI_LEFT) ||
           (pos0 == UI_LEFT && pos1 == UI_RIGHT) ||
           (pos0 == UI_RIGHT && pos1 == UI_RIGHT) ||
           (pos0 == UI_TOP && pos1 == UI_TOP) ||
           (pos0 == UI_BOTTOM && pos1 == UI_TOP) ||
           (pos0 == UI_BOTTOM && pos1 == UI_BOTTOM));
}

bool style_check_valid(const UiStyle *style) {
  if (!pos_valid(style->positions[0], style->positions[1]) ||
      !pos_valid(style->positions[1], style->positions[0])) {
    return false;
  }
  return true;
}

void layout_set_style(Layout *layout, UiStyle style) { layout->style = style; }

void layout_text(Layout *layout, TextComponent text) {
  UiComponent *component = &layout->components[layout->components_amount++];
  component->type = COMPONENT_TEXT;
  component->var.text = text;
}

void layout_text_simple(Layout *layout, const char *message) {
  layout_text(layout, (TextComponent){.message = message,
                                      .offset = {.x = 0, .y = 0},
                                      .color = WHITE,
                                      .dimensions =
                                          vec2i(MeasureText(message, 10), 10)});
}

void layout_button(Layout *layout, ButtonComponent button) {
  UiComponent *component = &layout->components[layout->components_amount++];
  component->type = COMPONENT_BUTTON;
  component->var.button = button;
}

void layout_button_simple(Layout *layout, const char *message,
                          ButtonClickFunction on_click_func);

void layout_spacing(Layout *layout, SpacingComponent spacing) {
  UiComponent *component = &layout->components[layout->components_amount++];
  component->type = COMPONENT_SPACING;
  component->var.spacing = spacing;
}

void layout_spacing_simple(Layout *layout, int spacing);
