#include "../include/ui.h"
#include "../include/config.h"
#include "raylib.h"
#include <stdbool.h>

void ui_set_background(UiRenderer *renderer, BackgroundUiComponent component) {

  int x = (renderer->context.screen_width - component.texture.width * 4.5) / 2;
  int y = (renderer->context.screen_height - component.texture.height * 4.5) / 2;
  if (renderer->cur_style.positions[0] == UI_LEFT || renderer->cur_style.positions[1] == UI_LEFT) {
    x = 0;
  }

  if (renderer->cur_style.positions[0] == UI_TOP || renderer->cur_style.positions[1] == UI_TOP) {
    y = 0;
  }

  if (renderer->cur_style.positions[0] == UI_BOTTOM || renderer->cur_style.positions[1] == UI_BOTTOM) {
    y = renderer->context.screen_height - component.texture.height * 4.5;
  }
  int w = component.texture.width * 4.5;
  DrawTextureEx(component.texture, (Vector2){x + component.x_offset, y + component.y_offset}, 0, 4.5, WHITE);
}

void ui_setup(UiRenderer *renderer, UiStyle ui_style) {
  renderer->groups_amount = 0;

  renderer->initial_style = ui_style;
  renderer->cur_style = ui_style;

  if (renderer->ui_height != -1) {
    renderer->cur_x = renderer->context.screen_width / 2;
    renderer->cur_y = (renderer->context.screen_height - renderer->ui_height) / 2;
  }
  switch (renderer->cur_style.alignment) {
  case UI_VERTICAL: {
    if (renderer->cur_style.positions[0] == UI_LEFT || renderer->cur_style.positions[1] == UI_LEFT) {
      renderer->cur_x = 0;
    }

    if (renderer->cur_style.positions[0] == UI_TOP || renderer->cur_style.positions[1] == UI_TOP) {
      renderer->cur_y = 0;
    }
    break;
  }
  case UI_HORIZONTAL: {
    if (renderer->cur_style.positions[0] == UI_LEFT || renderer->cur_style.positions[1] == UI_LEFT) {
      renderer->cur_x = 0;
    }

    if (renderer->cur_style.positions[0] == UI_TOP || renderer->cur_style.positions[1] == UI_TOP) {
      renderer->cur_y = 0;
    }
    break;
  }
  }

  renderer->context.screen_width = GetScreenWidth();
  renderer->context.screen_height = GetScreenHeight();
}

float ui_scale(UiRenderer *renderer) { return ((float)CONFIG.default_font_size / 10) * renderer->cur_style.scale; }

// COMPONENTS

// BUTTONS

void ui_button_render(UiRenderer *renderer, ButtonUiComponent component) {
  if (component.width == 0) {
    component.width = component.texture.width;
  }

  if (component.height == 0) {
    component.height = component.texture.height;
  }

  float scale = renderer->cur_style.scale * ui_scale(renderer);
  UiStyle style = renderer->cur_style;
  switch (renderer->cur_style.alignment) {
  case UI_VERTICAL: {
    if (style.positions[0] == UI_CENTER || style.positions[1] == UI_CENTER) {
      renderer->cur_x = (renderer->context.screen_width - component.width * scale) / 2;
    }
    renderer->cur_x += component.x_offset;
    break;
  }
  case UI_HORIZONTAL: {
    renderer->cur_x += component.x_offset;
    break;
  }
  }

  renderer->cur_x += component.x_offset;
  renderer->cur_y += component.y_offset;

  bool hovered = CheckCollisionPointRec(GetMousePosition(),
                                        (Rectangle){.x = renderer->cur_x,
                                                    .y = renderer->cur_y,
                                                    .width = component.width * scale,
                                                    .height = component.height * scale});
  DrawTexturePro(hovered ? component.selected_texture : component.texture,
                 (Rectangle){.x = 0, .y = 0, .width = component.texture.width, .height = component.texture.height},
                 (Rectangle){.x = renderer->cur_x + (component.width * scale) / 2,
                             .y = renderer->cur_y + (component.height * scale) / 2,
                             .width = component.width * scale,
                             .height = component.height * scale},
                 (Vector2){.x = (component.width * scale) / 2, .y = (component.height * scale) / 2}, 0, WHITE);
  int text_width = MeasureText(component.message, renderer->cur_style.font_scale);
  float text_x = renderer->cur_x + component.text_x_offset + (float)(component.width * scale - text_width) / 2;
  float text_y = renderer->cur_y + ((component.height * scale) / 2 - (float)renderer->cur_style.font_scale / 2) +
      component.text_y_offset;
  DrawText(component.message, text_x, text_y, renderer->cur_style.font_scale, WHITE);

  if (renderer->cur_style.alignment == UI_VERTICAL) {
  } else if (renderer->cur_style.alignment == UI_HORIZONTAL) {
  }

  switch (renderer->cur_style.alignment) {
  case UI_VERTICAL: {
    renderer->cur_y += component.height * scale + renderer->cur_style.padding;
    break;
  }
  case UI_HORIZONTAL: {
    renderer->cur_x += component.width * scale + renderer->cur_style.padding;
    break;
  }
  }

  if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    switch (component.on_click_func.func_type) {
    case BUTTON_CLICK_FUNC_NO_ARGS: {
      component.on_click_func.func_var.on_click_no_args();
      break;
    }
    case BUTTON_CLICK_FUNC_WITH_ARGS: {
      component.on_click_func.func_var.on_click_with_args(component.on_click_func.args);
      break;
    }
    }
  }
}

// TEXT

void ui_text_render(UiRenderer *renderer, TextUiComponent component) {
  Color color = component.color;
  if (component.color.r == 0 && component.color.g == 0 && component.color.b == 0 && component.color.a == 0) {
    color = WHITE;
  }
  int width = MeasureText(component.text, renderer->cur_style.font_scale);
  int height = renderer->cur_style.font_scale;
  switch (renderer->cur_style.alignment) {
  case UI_HORIZONTAL: {
    break;
  }
  case UI_VERTICAL: {
    renderer->cur_x = (renderer->context.screen_width - width) / 2;
    break;
  }
  }
  if (!renderer->simulate) {
    DrawText(component.text, renderer->cur_x + component.x_offset, renderer->cur_y + component.y_offset,
             renderer->cur_style.font_scale, color);
  }
  // TODO: Move cur coordinate by dimension depdening on style
  // renderer->cur_x += component->dimensions.x + component->offset.x;
  renderer->cur_y += height + renderer->cur_style.padding;
}

// TEXT INPUT

void ui_text_input_render(UiRenderer *renderer, TextInputUiComponent component) {
  if (component.width == 0) {
    component.width = component.texture.width;
  }

  if (component.height == 0) {
    component.height = component.texture.height;
  }

  bool selected = *component.selected;
  float scale = renderer->cur_style.scale * ui_scale(renderer);
  UiStyle style = renderer->cur_style;
  switch (renderer->cur_style.alignment) {
  case UI_VERTICAL: {
    if (style.positions[0] == UI_CENTER || style.positions[1] == UI_CENTER) {
      renderer->cur_x = (renderer->context.screen_width - component.width * scale) / 2;
    }
    renderer->cur_x += component.x_offset;
    break;
  }
  case UI_HORIZONTAL: {
    renderer->cur_x += component.x_offset;
    break;
  }
  }
  int x = renderer->cur_x + (component.width * scale) / 2;
  int y = renderer->cur_y + (component.height * scale) / 2;

  DrawTexturePro(component.texture,
                 (Rectangle){.x = 0, .y = 0, .width = component.texture.width, .height = component.texture.height},
                 (Rectangle){.x = x, .y = y, .width = component.width * scale, .height = component.height * scale},
                 (Vector2){.x = (component.width * scale) / 2, .y = (component.height * scale) / 2}, 0, WHITE);

  DrawText(component.text_input->buf, renderer->cur_x + 3 * scale + component.text_x_offset,
           renderer->cur_y + 3 + component.text_y_offset, renderer->cur_style.font_scale, WHITE);

  int line_x = renderer->cur_x + MeasureText(component.text_input->buf, renderer->cur_style.font_scale);

  if (((int)(GetTime() * 1.5)) % 2 == 0 && selected) {
    DrawLineEx(vec2f(line_x + 3 * scale, renderer->cur_y + (component.height - 3) * scale),
               vec2f(line_x + 11 * scale, renderer->cur_y + (component.height - 3) * scale), 3, WHITE);
  }

  if (selected) {
    KeyboardKey keycode_ch = GetCharPressed();
    if (keycode_ch != KEY_NULL) {
      char c = (char)keycode_ch;
      if (component.text_input->len < component.text_input->max_len) {
        component.text_input->buf[component.text_input->len++] = c;
      }
    }
  }

  if (IsKeyPressed(KEY_BACKSPACE) && selected) {
    if (component.text_input->len > 0) {
      component.text_input->buf[component.text_input->len - 1] = '\0';
      component.text_input->len--;
    }
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

    Rectangle text_input_box =
        rectf(renderer->cur_x, renderer->cur_y, component.width * scale, component.height * scale);
    (*component.selected) = CheckCollisionPointRec(GetMousePosition(), text_input_box);
  }

  switch (renderer->cur_style.alignment) {
  case UI_VERTICAL: {
    renderer->cur_y += component.height * scale + renderer->cur_style.padding;
    break;
  }
  case UI_HORIZONTAL: {
    renderer->cur_x += component.width * scale + renderer->cur_style.padding;
    break;
  }
  }
}

// SPACING

void ui_spacing_render(UiRenderer *renderer, SpacingUiComponent component) {
  renderer->cur_y += component.height + component.y_offset + renderer->cur_style.padding;
}

// GROUP

void ui_group_create(UiRenderer *renderer, GroupUiComponent component) {
  renderer->groups[renderer->groups_amount++] =
      (UiGroup){.component = component, .prev_x = renderer->cur_x, renderer->cur_y};
  renderer->cur_style = component.group_style;

  bool scissors = (component.width != -1 && component.height != -1);

  if (scissors) {
    BeginScissorMode(renderer->cur_x, renderer->cur_y, component.width, component.height);
  }

  if (component.scroll_y_offset != NULL) {
    int *scroll = component.scroll_y_offset;
    float wheel = GetMouseWheelMove();
    *scroll += (int)(wheel * 20); // Invert direction
    if (*scroll > 0)
      *scroll = 0; // Top of content (can't scroll past)
    if (*scroll < -200)
      *scroll = -200; // Bottom of content

    renderer->cur_y += *scroll;
  }
}

void ui_group_destroy(UiRenderer *renderer) {
  if (renderer->groups_amount <= 0) {
    TraceLog(LOG_WARNING, "Tried to destroy nonexistent ui group");
    return;
  }

  UiGroup group = renderer->groups[--renderer->groups_amount];

  bool scissors = (group.component.width != -1 && group.component.height != -1);

  if (scissors) {
    EndScissorMode();
    // renderer->cur_x  = group.prev_x + group.component.width;
    renderer->cur_y = group.prev_y + group.component.height;
  }

  UiStyle prev_ui_style = renderer->groups_amount > 0
      ? renderer->groups[renderer->groups_amount - 1].component.group_style
      : renderer->initial_style;
  renderer->cur_style = prev_ui_style;
}
