#include "../include/ui/ui_ecs.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static UiECS UI_ECS = {};

uint32_t ui_create_elem() { return UI_ECS.next_id++; }

void ui_elem_add_pos(uint32_t id, int x, int y) {
  UI_ECS.positions[id] = COMPONENT_CREATE(PositionComponent, {.x = 0, .y = 0});
}

void ui_elem_add_click_action(uint32_t id, ClickAction action) {
  UI_ECS.click_actions[id] = COMPONENT_CREATE(ClickActionComponent, {.action = action});
}

void ui_elem_add_text(uint32_t id, const char *text, int font_size, Color font_color) {
  UI_ECS.texts[id] = COMPONENT_CREATE(TextComponent, {.text = text, .font_size = font_size, .font_color = font_color});
}

void ui_elem_add_texture(uint32_t id, Texture2D texture) {
  UI_ECS.textures[id] = COMPONENT_CREATE(TextureComponent, {.texture = texture});
}

void ui_render() {
  for (int i = 0; i < UI_ECS.next_id; i++) {
    TextureComponent texture_component = UI_ECS.textures[i];
    if (texture_component.present) {
      int x = 0;
      int y = 0;
      PositionComponent pos_component = UI_ECS.positions[i];
      if (pos_component.present) {
        x = pos_component.val.x;
        y = pos_component.val.y;
      }
      DrawTexture(texture_component.val.texture, x, y, WHITE);
    }
  }
}

static void on_click() { printf("Hello, World\n"); }

void ui_build() {
  BUILD_LAYOUT({
    uint32_t ui_elem = ui_create_elem();
    ui_elem_add_pos(ui_elem, 10, 10);
    ui_elem_add_click_action(ui_elem, on_click);
    ui_elem_add_texture(ui_elem, LoadTexture("res/assets/gui/button.png"));
  });
}
