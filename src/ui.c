#include "../include/ui.h"
#include "../include/config.h"
#include "raylib.h"
#include <stdbool.h>

void ui_set_background(UiRenderer *renderer, Texture2D texture) {
  int w = texture.width * 4.5;
  DrawTextureEx(texture, (Vector2){((float)renderer->context.screen_width / 2) - ((float)w / 2), renderer->cur_y}, 0,
                4.5, WHITE);
}

void ui_setup(UiRenderer *renderer) {
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

void ui_set_style(UiRenderer *renderer, UiStyle style) { renderer->cur_style = style; }

float ui_scale(UiRenderer *renderer) { return ((float)CONFIG.default_font_size / 10) * renderer->cur_style.scale; }

// COMPONENTS

// BUTTONS

void ui_button_render_ex(UiRenderer *renderer, ButtonUiComponent component) {
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
    component.on_click_func();
  }
}

void ui_button_render(UiRenderer *renderer, const char *text, Texture2D texture, Texture2D selected_texture,
                      ButtonClickFunction on_click_func) {
  ui_button_render_ex(renderer,
                      (ButtonUiComponent){.message = text,
                                          .on_click_func = on_click_func,
                                          .texture = texture,
                                          .selected_texture = selected_texture,
                                          .width = texture.width,
                                          .height = texture.height,
                                          .x_offset = 0,
                                          .y_offset = 0,
                                          .text_x_offset = 0,
                                          .text_y_offset = -4});
}

void ui_button_render_dimensions_offset(UiRenderer *renderer, const char *text, Texture2D texture,
                                        Texture2D selected_texture, ButtonClickFunction on_click_func, Vec2i offset,
                                        Vec2i dimensions) {
  ui_button_render_ex(renderer,
                      (ButtonUiComponent){.message = text,
                                          .on_click_func = on_click_func,
                                          .texture = texture,
                                          .selected_texture = selected_texture,
                                          .width = dimensions.x,
                                          .height = dimensions.y,
                                          .x_offset = 0,
                                          .y_offset = 0,
                                          .text_x_offset = offset.x,
                                          .text_y_offset = offset.y - 4});
}

void ui_button_render_offset(UiRenderer *renderer, const char *text, Texture2D texture, Texture2D selected_texture,
                             ButtonClickFunction on_click_func, Vec2i offset) {
  ui_button_render_ex(renderer,
                      (ButtonUiComponent){.message = text,
                                          .on_click_func = on_click_func,
                                          .texture = texture,
                                          .selected_texture = selected_texture,
                                          .width = texture.width,
                                          .height = texture.height,
                                          .x_offset = 0,
                                          .y_offset = 0,
                                          .text_x_offset = offset.x,
                                          .text_y_offset = offset.y - 4});
}

// TEXT

void ui_text_render_ex(UiRenderer *renderer, TextUiComponent component) {
  renderer->cur_x = (renderer->context.screen_width - component.width) / 2;
  if (!renderer->simulate) {
    DrawText(component.text, renderer->cur_x + component.x_offset, renderer->cur_y + component.y_offset,
             renderer->cur_style.font_scale, component.color);
  }
  // TODO: Move cur coordinate by dimension depdening on style
  // renderer->cur_x += component->dimensions.x + component->offset.x;
  renderer->cur_y += component.height + renderer->cur_style.padding;
}

void ui_text_render_offset(UiRenderer *renderer, const char *text, Vec2i offset) {
  ui_text_render_ex(renderer,
                    (TextUiComponent){.text = text,
                                      .color = WHITE,
                                      .height = CONFIG.default_font_size,
                                      .width = MeasureText(text, CONFIG.default_font_size),
                                      .x_offset = offset.x,
                                      .y_offset = offset.y});
}

void ui_text_render(UiRenderer *renderer, const char *text) {
  ui_text_render_ex(renderer,
                    (TextUiComponent){.text = text,
                                      .color = WHITE,
                                      .height = CONFIG.default_font_size,
                                      .width = MeasureText(text, CONFIG.default_font_size),
                                      .x_offset = 0,
                                      .y_offset = 0});
}

// TEXT INPUT

void ui_text_input_render_ex(UiRenderer *renderer, TextInputUiComponent component) {
  float scale = ui_scale(renderer) * renderer->cur_style.scale;
  int x = renderer->cur_x + (component.width * scale) / 2;
  int y = renderer->cur_y + (component.height * scale) / 2;
  DrawTexturePro(component.texture,
                 (Rectangle){.x = 0, .y = 0, .width = component.texture.width, .height = component.texture.height},
                 (Rectangle){.x = x, .y = y, .width = component.width * scale, .height = component.height * scale},
                 (Vector2){.x = (component.width * scale) / 2, .y = (component.height * scale) / 2}, 0, WHITE);

  DrawText(component.text_input->buf, renderer->cur_x + 3 * scale, renderer->cur_y + 3,
           renderer->cur_style.font_scale, WHITE);

  int line_x = renderer->cur_x + MeasureText(component.text_input->buf, renderer->cur_style.font_scale);

  if (((int)(GetTime() * 1.5)) % 2 == 0) {
    DrawLineEx(vec2f(line_x + 3 * scale, renderer->cur_y + (component.height - 3) * scale),
               vec2f(line_x + 11 * scale, renderer->cur_y + (component.height - 3) * scale), 3, WHITE);
  }

  KeyboardKey keycode_ch = GetCharPressed();
  if (keycode_ch != KEY_NULL) {
    char c = (char)keycode_ch;
    if (component.text_input->len < component.text_input->max_len) {
      component.text_input->buf[component.text_input->len++] = c;
    }
  }

  if (IsKeyPressed(KEY_BACKSPACE)) {
    if (component.text_input->len > 0) {
      component.text_input->buf[component.text_input->len - 1] = '\0';
      component.text_input->len--;
    }
  }
}

void ui_text_input_render_dimensions(UiRenderer *renderer, Texture2D texture, TextInputBuffer *text_input_buf,
                                     Vec2i dimensions) {
  ui_text_input_render_ex(renderer,
                          (TextInputUiComponent){.texture = texture,
                                                 .text_input = text_input_buf,
                                                 .color = WHITE,
                                                 .width = dimensions.x,
                                                 .height = dimensions.y,
                                                 .x_offset = 0,
                                                 .y_offset = 0});
}

void ui_text_input_render(UiRenderer *renderer, Texture2D texture, TextInputBuffer *text_input_buf) {
  ui_text_input_render_dimensions(renderer, texture, text_input_buf, vec2i(texture.width, texture.height));
}

// SPACING

void ui_spacing_render_ex(UiRenderer *renderer, SpacingUiComponent component) {
  renderer->cur_y += component.height + component.y_offset + renderer->cur_style.padding;
}

void ui_spacing_render(UiRenderer *renderer, int spacing_height) {
  ui_spacing_render_ex(renderer,
                       (SpacingUiComponent){.height = spacing_height, .width = 100, .x_offset = 0, .y_offset = 0});
}
