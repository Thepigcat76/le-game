#include "../../include/config.h"
#include "../../include/net/server.h"
#include "../../include/net/client.h"
#include "../../include/ui.h"
#include <raylib.h>

static char ip_addr_buf[128];
static TextInputBuffer IP_ADDR = {.buf = ip_addr_buf, .len = 0, .max_len = 128};
static bool ip_addr_input_selected = false;

static void host_menu_host() { server_start(IP_ADDR.buf, DEBUG_PORT); }

static void host_menu_back() { client_set_menu(&CLIENT_GAME, MENU_MULTIPLAYER); }

void host_menu_open(UiRenderer *renderer, const ClientGame *game) { ip_addr(IP_ADDR.buf); }

void host_menu_render(UiRenderer *renderer, const ClientGame *game) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 24,
      .scale = 1,
      .font_scale = CONFIG.default_font_size,
  });

  int x_offset = 0;
  int y_offset = -2;

  RENDER_TEXT({.text = "Host server"});
  RENDER_SPACING({.height = 40});
  RENDER_TEXT_INPUT({
      .text_input = &IP_ADDR,
      .texture = TEXT_INPUT_TEXTURE,
      .selected = &ip_addr_input_selected,
      .width = 128,
  });
  RENDER_BUTTON({
      .message = "Host",
      .texture = BUTTON_TEXTURE,
      .selected_texture = BUTTON_SELECTED_TEXTURE,
      .on_click_func = button_click_simple(host_menu_host),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Back",
      .texture = BUTTON_TEXTURE,
      .selected_texture = BUTTON_SELECTED_TEXTURE,
      .on_click_func = button_click_simple(host_menu_back),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
}
