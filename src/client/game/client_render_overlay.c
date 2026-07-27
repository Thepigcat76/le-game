#include "../../../include/config.h"
#include "../../../include/game.h"
#include "../../../include/net/client.h"
#include "rlgl.h"
#include <lilc/log.h>
#include <raylib.h>

#define RENDER_MENU(ui_renderer, menu_name)                                                                                                \
  extern void menu_name##_render(UiRenderer *renderer, const ClientGame *client);                                                          \
  menu_name##_render(ui_renderer, client);

static void text_draw_centered_x(const char *text, i32 x, i32 y, i32 font_size, Color color) {
  i32 text_width = MeasureText(text, font_size);
  DrawText(text, x - text_width / 2, y, font_size, color);
}

void client_render_overlay(ClientGame *client) {
  Vec2i pos = vec2i(GetScreenWidth() - (3.5 * 16) - 30, (GetScreenHeight() / 2.0f) - (3.5 * 8));

  cw_Texture main_hand_slot_tex = tex_by_handle(&client->asset_manager, TEX_MAIN_HAND_SLOT);
  DrawTextureEx(main_hand_slot_tex.texture, (Vector2){pos.x, pos.y}, 0, 4.5, WHITE);
  item_render(&CLIENT_PLAYER->held_item, pos.x + 2 * 3.5, pos.y + 2 * 3.5);

  debug_render_overlay(&client->game.debug);

  Vec2f mouse_pos = GetMousePosition();
  if (client->state.slot_selected) {
    cw_Texture tooltip_texture = tex_by_handle(&client->asset_manager, TEX_TOOLTIP);
    cw_Shader tooltip_shader = cw_shader_by_handle(&client->asset_manager, SHADER_TOOLTIP);
    f32 scale = 2.0f;
    f32 base_scale = 3.0f;
    Vec2i tooltip_render_pos = vec2i((mouse_pos.x - tooltip_texture.width * scale * base_scale), mouse_pos.y);
    Vec2i tooltip_render_pos_scaled =
        vec2i((mouse_pos.x - tooltip_texture.width * scale * base_scale) / base_scale, mouse_pos.y / base_scale);

    BeginShaderMode(tooltip_shader.shader);
    {
      SetShaderValue(tooltip_shader.shader, tooltip_shader.shader_values_locs[RESOLUTION_LOC],
                     (f32[2]){
                         tooltip_texture.width,
                         tooltip_texture.height,
                     },
                     SHADER_UNIFORM_VEC2);
      rlPushMatrix();
      {
        rlScalef(base_scale, base_scale, 1);
        tex_draw1(&tooltip_texture, tooltip_render_pos_scaled, scale, WHITE);
        // tex_draw3(&tooltip_texture, rectf(tooltip_pos.x, tooltip_pos.y, tooltip_texture.width * 2, tooltip_texture.height * 2),
        //           rectf(0, 0, tooltip_texture.width, tooltip_texture.height), vec2f(tooltip_texture.width / 2.0f, tooltip_texture.height
        //           / 2.0f), 0, WHITE);
      }
      rlPopMatrix();

      i32 y_offset = 15;
      ItemProperties held_item_props = client->game.registries.items[CLIENT_PLAYER->held_item.id];
      char *name = held_item_props.name;

      i32 tooltip_center_x = tooltip_render_pos.x + (tooltip_texture.width * scale * base_scale) / 2;
      text_draw_centered_x(name, tooltip_center_x, tooltip_render_pos.y + y_offset, CONFIG.default_font_size, WHITE);

      char tooltip[256];
      item_tooltip(&CLIENT_PLAYER->held_item, tooltip, 256);
      i32 tooltip_lines_count;
      char **tooltip_lines = TextSplit(tooltip, '\n', &tooltip_lines_count);
      log_debug("Tooltip: %s, Lines: %d", tooltip, tooltip_lines_count);
      for (i32 i = 0; i < tooltip_lines_count; i++) {
        text_draw_centered_x(tooltip_lines[i], tooltip_center_x, tooltip_render_pos.y + y_offset + (i + 1) * CONFIG.default_font_size,
                 CONFIG.default_font_size, WHITE);
      }
    }
    EndShaderMode();
  }
}

void client_render_menu(ClientGame *client) {
  UiRenderer *ui_renderer = &client->ui_renderer;

  MenuProperties menu_props = client->game.registries.menus[client->state.cur_menu];

  if (menu_props.render_func != 0) {
    menu_props.render_func(ui_renderer, client);
  }
}
