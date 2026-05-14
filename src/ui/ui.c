#include "../../include/ui.h"
#include "../../include/net/client.h"
#include "../../include/shared.h"
#include "lilc/log.h"
#include "raylib.h"
#include <lilc/alloc.h>
#include <stdbool.h>

void ui_renderer_init(UiRenderer *renderer) {
  renderer->cur_x = 0;
  renderer->cur_y = 0;
  renderer->simulate = false;
  renderer->ui_width = -1;
  renderer->ui_height = -1;
  renderer->cur_style = (UiStyle){0};
  renderer->initial_style = (UiStyle){0};
  renderer->context = (UiContext){.screen_width = GetScreenWidth(), .screen_height = GetScreenHeight()};

  bump_init(&renderer->ui_bump, 1024);
  bump_allocator_init(&renderer->ui_bump_allocator, &renderer->ui_bump);
}

void ui_set_background(UiRenderer *renderer, BackgroundUiComponent component) {
  i32 x = (renderer->context.screen_width - component.texture.width * 4.5) / 2;
  i32 y = (renderer->context.screen_height - component.texture.height * 4.5) / 2;
  if (renderer->cur_style.positions[0] == UI_LEFT || renderer->cur_style.positions[1] == UI_LEFT) {
    x = 0;
  }

  if (renderer->cur_style.positions[0] == UI_TOP || renderer->cur_style.positions[1] == UI_TOP) {
    y = 0;
  }

  if (renderer->cur_style.positions[0] == UI_BOTTOM || renderer->cur_style.positions[1] == UI_BOTTOM) {
    y = renderer->context.screen_height - component.texture.height * 4.5;
  }
  DrawTextureEx(component.texture, (Vector2){x + component.x_offset, y + component.y_offset}, 0, 4.5, WHITE);
}

void ui_setup(UiRenderer *renderer, UiStyle ui_style) {
  renderer->groups_amount = 0;

  renderer->initial_style = ui_style;
  renderer->cur_style = ui_style;

  if (renderer->ui_height != -1 || renderer->ui_height != -1) {
    renderer->cur_x = (renderer->context.screen_width - renderer->ui_width) / 2;
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

float ui_scale(UiRenderer *renderer) { return renderer->cur_style.scale * (renderer->cur_style.font_scale / 10); }

// COMPONENTS

// BUTTONS

static void move_ex(UiRenderer *renderer, i32 width, i32 height, bool scale_x, bool scale_y) {
  float scale = renderer->cur_style.scale * ui_scale(renderer);
  switch (renderer->cur_style.alignment) {
  case UI_VERTICAL: {
    if (scale_y) {
      renderer->cur_y += height * scale + renderer->cur_style.padding;
    } else {
      renderer->cur_y += height + renderer->cur_style.padding;
    }
    break;
  }
  case UI_HORIZONTAL: {
    if (scale_x) {
      renderer->cur_x += width * scale + renderer->cur_style.padding;
    } else {
      renderer->cur_x += width + renderer->cur_style.padding;
    }
    break;
  }
  }
}

static void move(UiRenderer *renderer, i32 width, i32 height) { move_ex(renderer, width, height, true, true); }

static void align_ex(UiRenderer *renderer, i32 width, i32 height, i32 x_offset, i32 y_offset, bool scale_horizontal, bool scale_vertical) {
  UiStyle style = renderer->cur_style;
  float scale = renderer->cur_style.scale * ui_scale(renderer);
  switch (renderer->cur_style.alignment) {
  case UI_VERTICAL: {
    if (style.positions[0] == UI_CENTER || style.positions[1] == UI_CENTER) {
      if (scale_vertical) {
        renderer->cur_x = (renderer->context.screen_width - width * scale) / 2;
      } else {
        renderer->cur_x = (renderer->context.screen_width - width) / 2;
      }
    }
    renderer->cur_x += x_offset;
    break;
  }
  case UI_HORIZONTAL: {
    if (style.positions[0] == UI_CENTER || style.positions[1] == UI_CENTER) {
      if (scale_horizontal) {
        renderer->cur_y = (renderer->context.screen_height - height * scale) / 2;
      } else {
        renderer->cur_y = (renderer->context.screen_height - height) / 2;
      }
    }
    renderer->cur_y += y_offset;
    break;
  }
  }
}

static void align(UiRenderer *renderer, i32 width, i32 height, i32 x_offset, i32 y_offset) {
  align_ex(renderer, width, height, x_offset, y_offset, true, true);
}

void ui_button_render(UiRenderer *renderer, ButtonUiComponent component) {
  TextureHandle texture = component.texture.present ? component.texture.texture_handle : TEX_BUTTON;
  TextureHandle selected_texture = component.selected_texture.present ? component.selected_texture.texture_handle : TEX_BUTTON_SELECTED;

  cw_Texture tex = cw_tex_by_handle(renderer->asset_manager, texture);

  if (component.width == 0) {
    component.width = tex.width;
  }

  if (component.height == 0) {
    component.height = tex.height;
  }

  float scale = renderer->cur_style.scale * ui_scale(renderer);
  align(renderer, component.width, component.height, component.x_offset, component.y_offset);

  renderer->cur_x += component.x_offset;
  renderer->cur_y += component.y_offset;

  bool hovered = CheckCollisionPointRec(
      GetMousePosition(),
      (Rectangle){.x = renderer->cur_x, .y = renderer->cur_y, .width = component.width * scale, .height = component.height * scale});
  Texture2D final_tex = tex_by_handle(renderer->asset_manager, hovered ? selected_texture : texture);
  DrawTexturePro(final_tex, (Rectangle){.x = 0, .y = 0, .width = tex.width, .height = tex.height},
                 (Rectangle){.x = renderer->cur_x + (component.width * scale) / 2,
                             .y = renderer->cur_y + (component.height * scale) / 2,
                             .width = component.width * scale,
                             .height = component.height * scale},
                 (Vector2){.x = (component.width * scale) / 2, .y = (component.height * scale) / 2}, 0, WHITE);
  i32 text_width = MeasureText(component.message, renderer->cur_style.font_scale);

  f32 text_x = renderer->cur_x + component.text_x_offset + (float)(component.width * scale - text_width) / 2;
  f32 text_y = renderer->cur_y + ((component.height * scale) / 2 - (float)renderer->cur_style.font_scale / 2) + component.text_y_offset;
  DrawText(component.message, text_x, text_y, renderer->cur_style.font_scale, WHITE);

  if (renderer->cur_style.alignment == UI_VERTICAL) {
  } else if (renderer->cur_style.alignment == UI_HORIZONTAL) {
  }

  move(renderer, component.width, component.height);

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

  i32 width = MeasureText(component.text, renderer->cur_style.font_scale);
  i32 height = renderer->cur_style.font_scale;

  align_ex(renderer, width, height, component.x_offset, component.y_offset, false, false);

  if (!renderer->simulate) {
    DrawText(component.text, renderer->cur_x + component.x_offset, renderer->cur_y + component.y_offset, renderer->cur_style.font_scale,
             color);
  }

  move_ex(renderer, width, height, false, false);
}

// TEXT INPUT

void ui_text_input_render(UiRenderer *renderer, TextInputUiComponent component) {
  cw_Texture tex = cw_tex_by_handle(renderer->asset_manager, component.texture.present ? component.texture.texture_handle : TEX_TEXT_INPUT);

  if (component.width == 0) {
    component.width = tex.width;
  }

  if (component.height == 0) {
    component.height = tex.height;
  }

  bool selected = *component.selected;
  float scale = renderer->cur_style.scale * ui_scale(renderer);
  UiStyle style = renderer->cur_style;
  align(renderer, component.width, component.height, component.x_offset, component.y_offset);
  i32 x = renderer->cur_x + (component.width * scale) / 2;
  i32 y = renderer->cur_y + (component.height * scale) / 2;

  Texture2D texture = tex_by_id(renderer->asset_manager, tex.id);

  DrawTexturePro(texture, (Rectangle){.x = 0, .y = 0, .width = tex.width, .height = tex.height},
                 (Rectangle){.x = x, .y = y, .width = component.width * scale, .height = component.height * scale},
                 (Vector2){.x = (component.width * scale) / 2, .y = (component.height * scale) / 2}, 0, WHITE);

  DrawText(component.text_input->buf, renderer->cur_x + 3 * scale + component.text_x_offset, renderer->cur_y + 3 + component.text_y_offset,
           renderer->cur_style.font_scale, WHITE);

  i32 line_x = renderer->cur_x + MeasureText(component.text_input->buf, renderer->cur_style.font_scale);

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

    Rectangle text_input_box = rectf(renderer->cur_x, renderer->cur_y, component.width * scale, component.height * scale);
    (*component.selected) = CheckCollisionPointRec(GetMousePosition(), text_input_box);
  }

  move(renderer, component.width, component.height);
}

// SPACING

void ui_spacing_render(UiRenderer *renderer, SpacingUiComponent component) {
  renderer->cur_x += component.width + component.x_offset + renderer->cur_style.padding;
  renderer->cur_y += component.height + component.y_offset + renderer->cur_style.padding;
}

// SLOT

void ui_slot_render(UiRenderer *renderer, SlotUiComponent component) {
  if (component.width == 0) {
    component.width = 16;
  }

  if (component.height == 0) {
    component.height = 16;
  }

  align(renderer, component.width, component.height, component.x_offset, component.y_offset);

  i32 width = component.width * ui_scale(renderer);
  i32 height = component.height * ui_scale(renderer);

  if (CheckCollisionPointRec(GetMousePosition(), rectf(renderer->cur_x, renderer->cur_y, width, height)) && !component.fake) {
    DrawRectangle(renderer->cur_x, renderer->cur_y, width, height, color_rgba(150, 150, 150, 150));

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      if (item_is_empty(&CLIENT_PLAYER->dragged_item)) {
        CLIENT_PLAYER->dragged_item = *component.item;
        *component.item = ITEM_INSTANCE_EMPTY;
      } else if (item_is_empty(component.item)) {
        *component.item = CLIENT_PLAYER->dragged_item;
        CLIENT_PLAYER->dragged_item = ITEM_INSTANCE_EMPTY;
      } else {
        ItemInstance dragged_item = CLIENT_PLAYER->dragged_item;
        CLIENT_PLAYER->dragged_item = *component.item;
        *component.item = dragged_item;
      }
    }
  }

  if (component.item != NULL) {
    item_render(component.item, renderer->cur_x, renderer->cur_y);
  }

  Texture2D tex = tex_by_handle(renderer->asset_manager, TEX_SLOT);
  DrawTextureEx(tex, vec2f(renderer->cur_x - 2 * ui_scale(renderer), renderer->cur_y - 2 * ui_scale(renderer)), 0,
                 ui_scale(renderer), WHITE);

  move(renderer, component.width, component.height);
}

// GROUP

void ui_group_create(UiRenderer *renderer, GroupUiComponent component) {
  renderer->groups[renderer->groups_amount++] = (UiGroup){.component = component, .prev_x = renderer->cur_x, renderer->cur_y};
  renderer->cur_style = component.group_style;

  bool scissors = (component.width != -1 && component.height != -1);

  if (scissors) {
    BeginScissorMode(renderer->cur_x, renderer->cur_y, component.width, component.height);
  }

  if (component.scroll_y_offset != NULL) {
    i32 *scroll = component.scroll_y_offset;
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
    renderer->cur_x = group.prev_x + group.component.width;
    renderer->cur_y = group.prev_y + group.component.height;
  }

  UiStyle prev_ui_style =
      renderer->groups_amount > 0 ? renderer->groups[renderer->groups_amount - 1].component.group_style : renderer->initial_style;
  renderer->cur_style = prev_ui_style;
}
