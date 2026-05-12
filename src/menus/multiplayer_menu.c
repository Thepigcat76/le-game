#include "menu_includes.h"
#include "../../include/net/payloads.h"
#include <raylib.h>

static void multiplayer_menu_join() {
  addr_t server_addr = client_join_server(&CLIENT_GAME, "127.0.0.1", 12345);

  PayloadClientConnect payload = {.client_name = "Dev"};
  packet_send(server_addr, C2S_CLIENT_CONNECT, &payload);
}

static void multiplayer_menu_host() { client_set_menu(&CLIENT_GAME, MENU_HOST_SERVER); }

static void multiplayer_menu_back() { client_set_menu(&CLIENT_GAME, MENU_START); }

void multiplayer_menu_render(UiRenderer *renderer, const ClientGame *game) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 24,
      .scale = 1,
      .font_scale = CONFIG.default_font_size,
  });

  int x_offset = 0;
  int y_offset = -2;

  RENDER_TEXT({.text = "Multiplayer"});
  RENDER_SPACING({.height = 40});
  RENDER_BUTTON({
      .message = "Join",
      .on_click_func = button_click_simple(multiplayer_menu_join),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Host",
      .on_click_func = button_click_simple(multiplayer_menu_host),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Back",
      .on_click_func = button_click_simple(multiplayer_menu_back),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
}
