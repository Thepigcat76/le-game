#include "../../include/config.h"
#include "../../include/game.h"

#define RENDER_MENU(ui_renderer, menu_name)                                                                            \
  extern void menu_name##_render(UiRenderer *renderer, const ClientGame *client);                                              \
  menu_name##_render(ui_renderer, client);

void client_render_overlay(ClientGame *client) {
  Vec2i pos = vec2i(GetScreenWidth() - (3.5 * 16) - 30, (GetScreenHeight() / 2.0f) - (3.5 * 8));
  DrawTextureEx(MAIN_HAND_SLOT_TEXTURE, (Vector2){pos.x, pos.y}, 0, 4.5, WHITE);
  item_render(&client->player->held_item, pos.x + 2 * 3.5, pos.y + 2 * 3.5);

#ifdef SURTUR_DEBUG
  debug_render_overlay();
#endif

  if (client->slot_selected) {
    Vec2f mouse_pos = GetMousePosition();
    if (mouse_pos.x + client->texture_manager.textures[TEXTURE_TOOLTIP].width * 5 > GetScreenWidth()) {
      mouse_pos.x -= client->texture_manager.textures[TEXTURE_TOOLTIP].width * 5;
    }
    BeginShaderMode(client->shader_manager.shaders[SHADER_TOOLTIP_OUTLINE]);
    {
      SetShaderValue(client->shader_manager.shaders[SHADER_TOOLTIP_OUTLINE],
                     GetShaderLocation(client->shader_manager.shaders[SHADER_TOOLTIP_OUTLINE], "resolution"),
                     (float[2]){client->texture_manager.textures[TEXTURE_TOOLTIP].width,
                                client->texture_manager.textures[TEXTURE_TOOLTIP].height},
                     SHADER_UNIFORM_VEC2);
      DrawTextureEx(client->texture_manager.textures[TEXTURE_TOOLTIP], mouse_pos, 0, 5, WHITE);
    }
    EndShaderMode();

    int y_offset = 15;
    char *name = item_type_to_string(&client->player->held_item.type);
    DrawText(name,
             mouse_pos.x +
                 ((float)client->texture_manager.textures[TEXTURE_TOOLTIP].width * 5 -
                  MeasureText(name, CONFIG.default_font_size)) /
                     2,
             mouse_pos.y + y_offset, CONFIG.default_font_size, WHITE);
    char tooltip[256];
    item_tooltip(&client->player->held_item, tooltip, 256);
    int count;
    const char **tooltip_lines = TextSplit(tooltip, '\n', &count);
    for (int i = 0; i < count; i++) {
      DrawText(tooltip_lines[i],
               mouse_pos.x +
                   ((float)client->texture_manager.textures[TEXTURE_TOOLTIP].width * 5 -
                    MeasureText(tooltip_lines[i], CONFIG.default_font_size)) /
                       2,
               mouse_pos.y + y_offset + (CONFIG.default_font_size * (i + 1)), CONFIG.default_font_size, WHITE);
    }
  }
}

void client_render_menu(ClientGame *client) {
  UiRenderer *ui_renderer = &client->ui_renderer;
  switch (client->cur_menu) {
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