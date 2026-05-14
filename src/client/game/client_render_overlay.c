#include "../../../include/config.h"
#include "../../../include/debug.h"
#include "../../../include/game.h"
#include "../../../include/net/client.h"

#define RENDER_MENU(ui_renderer, menu_name)                                                                                                \
  extern void menu_name##_render(UiRenderer *renderer, const ClientGame *client);                                                          \
  menu_name##_render(ui_renderer, client);

void client_render_overlay(ClientGame *client) {
  Vec2i pos = vec2i(GetScreenWidth() - (3.5 * 16) - 30, (GetScreenHeight() / 2.0f) - (3.5 * 8));
  
  Texture2D main_hand_slot_tex = tex_by_handle(&client->asset_manager, TEX_MAIN_HAND_SLOT);
  DrawTextureEx(main_hand_slot_tex, (Vector2){pos.x, pos.y}, 0, 4.5, WHITE);
  item_render(&CLIENT_PLAYER->held_item, pos.x + 2 * 3.5, pos.y + 2 * 3.5);

#ifdef DEBUG_BUILD
  debug_render_overlay(&client->game.debug);
#endif

  Vec2f mouse_pos = GetMousePosition();
  if (client->state.slot_selected) {
    // TODO: Reenable

    // if (mouse_pos.x + client->texture_manager.textures[TEXTURE_TOOLTIP].width * 5 > GetScreenWidth()) {
    //   mouse_pos.x -= client->texture_manager.textures[TEXTURE_TOOLTIP].width * 5;
    // }
    //
    //;
    //
    Texture2D tooltip_texture = tex_by_handle(&client->asset_manager, TEX_TOOLTIP);
    cw_Shader tooltip_shader = cw_shader_by_handle(&client->asset_manager, SHADER_TOOLTIP);

    BeginShaderMode(tooltip_shader.shader);
    {
      mouse_pos.x -= tooltip_texture.width * 5;
      SetShaderValue(tooltip_shader.shader, tooltip_shader.shader_values_locs[RESOLUTION_LOC],
                     (float[2]){tooltip_texture.width, tooltip_texture.height}, SHADER_UNIFORM_VEC2);
      DrawTextureEx(tooltip_texture, mouse_pos, 0, 5, WHITE);
    }
    EndShaderMode();

    int y_offset = 15;
    char *name = item_type_to_string(&CLIENT_PLAYER->held_item.type);
    // DrawText(name,
    //          mouse_pos.x +
    //              ((float)client->texture_manager.textures[TEXTURE_TOOLTIP].width * 5 - MeasureText(name, CONFIG.default_font_size)) / 2,
    //          mouse_pos.y + y_offset, CONFIG.default_font_size, WHITE);
    char tooltip[256];
    item_tooltip(&CLIENT_PLAYER->held_item, tooltip, 256);
    int count;
    char **tooltip_lines = TextSplit(tooltip, '\n', &count);
    for (int i = 0; i < count; i++) {
      // DrawText(tooltip_lines[i],
      //          mouse_pos.x +
      //              ((float)client->texture_manager.textures[TEXTURE_TOOLTIP].width * 5 -
      //               MeasureText(tooltip_lines[i], CONFIG.default_font_size)) /
      //                  2,
      //          mouse_pos.y + y_offset + (CONFIG.default_font_size * (i + 1)), CONFIG.default_font_size, WHITE);
    }
  }
}

void client_render_menu(ClientGame *client) {
  UiRenderer *ui_renderer = &client->ui_renderer;
  switch (client->state.cur_menu) {
  case MENU_SAVE: {
    RENDER_MENU(ui_renderer, save_menu);
    break;
  }
  case MENU_START: {
    RENDER_MENU(ui_renderer, start_menu);
    break;
  }
  case MENU_BACKPACK: {
    RENDER_MENU(ui_renderer, backpack_menu);
    break;
  }
  case MENU_DEBUG: {
    RENDER_MENU(ui_renderer, debug_menu);
    break;
  }
  case MENU_NEW_SAVE: {
    RENDER_MENU(ui_renderer, new_save_menu);
    break;
  }
  case MENU_LOAD_SAVE: {
    RENDER_MENU(ui_renderer, load_save_menu);
    break;
  }
  case MENU_MAP: {
    RENDER_MENU(ui_renderer, map_menu);
    break;
  }
  case MENU_DIALOG: {
    RENDER_MENU(ui_renderer, dialog_menu);
    break;
  }
  case MENU_INVENTORY: {
    RENDER_MENU(ui_renderer, inventory_menu);
    break;
  }
  case MENU_MULTIPLAYER: {
    RENDER_MENU(ui_renderer, multiplayer_menu);
    break;
  }
  case MENU_HOST_SERVER: {
    RENDER_MENU(ui_renderer, host_menu);
    break;
  }
  case MENU_NONE: {
    break;
  }
  }
}
