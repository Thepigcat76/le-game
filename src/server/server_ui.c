#include <lilc/alloc.h>
#include <lilc/dir.h>
#include <raylib.h>
#define CTX_SERVER
#include "../../include/ui.h"
#undef CTX_SERVER
#include "../../include/game.h"
#include "../../include/net/packet.h"
#include "../../include/net/payloads.h"
#include "../../include/net/server.h"
#include "../../include/save_desc.h"
#include "../../include/server_ui.h"
#include "../../include/shared.h"
#include "../../vendor/cJSON.h"
#include "lilc/array.h"
#include "lilc/log.h"

static AssetManager asset_manager = {0};

void server_ui_setup(UiRenderer *renderer) {
  renderer->asset_manager = &asset_manager;

  assets_load(&asset_manager);
}

static void on_click(void) {
  printf("Button pressed\n");
  if (!DirectoryExists("server-save")) {
    dir_create("server-save");
  }
  SaveConfig config = {.save_name = generate_save_name(&HEAP_ALLOCATOR).string, .seed = string_to_world_seed("")};
  SaveDescriptor desc = {.id = 0, .config = config, .is_server_save = true};
  cJSON *json = save_config_to_json(&config);
  char *file_content = cJSON_Print(json);
  FILE *f = fopen("server-save/game.json", "w");
  fputs(file_content, f);
  fclose(f);
  cJSON_Delete(json);
  free(file_content);
  if (!DirectoryExists("server-save/spaces")) {
    dir_create("server-save/spaces");
  }
  Save save = save_new(desc);
  Space default_space;
  printf("Seed: %f\n", config.seed);
  space_init_default(&default_space, config.seed);
  array_add(save.loaded_spaces, default_space);
  SERVER_GAME.game.cur_save = save;
  for (size_t i = 0; i < array_len(SERVER_GAME.clients); i++) {
    addr_t addr = SERVER_GAME.clients[i].address;
    log_debug("Sending sync space packet to client: %d", addr);

    PayloadSyncSpace payload = {.space = default_space};
    packet_send(addr, S2C_SYNC_SPACE, &payload);
  }
}

void server_ui_render(UiRenderer *renderer, ServerGame *server) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_HORIZONTAL,
      .padding = 24,
      .scale = 1,
      .font_scale = 24,
  });

  UI_GROUP_CREATE({
      .group_style =
          {
              .positions = {UI_CENTER, UI_CENTER},
              .alignment = UI_VERTICAL,
              .padding = 24,
              .scale = 1,
              .font_scale = 24,
          },
      .width = renderer->context.screen_width / 2,
      .height = renderer->context.screen_height,
  });
  {
    RENDER_TEXT({.text = "Server"});
    RENDER_TEXT({.text = TextFormat("Connected clients: %zu", array_len(server->clients))});
    for (size_t i = 0; i < array_len(server->clients); i++) {
      RENDER_TEXT({.text = TextFormat("- Addr: %u - %s", server->clients[i].address, server->clients[i].name)});
    }
    RENDER_BUTTON({.message = "Create save", .on_click_func = button_click_simple(on_click), .text_y_offset = -4});
  }
  UI_GROUP_DESTROY();

  UI_GROUP_CREATE({
      .group_style =
          {
              .positions = {UI_CENTER, UI_CENTER},
              .alignment = UI_VERTICAL,
              .padding = 24,
              .scale = 1,
              .font_scale = 24,
          },
      .width = renderer->context.screen_width / 2,
      .height = renderer->context.screen_height,
  });
  {
    RENDER_TEXT({.text = "Ballz"});
    RENDER_TEXT({.text = TextFormat("Connected clients: %zu", array_len(server->clients))});
    for (size_t i = 0; i < array_len(server->clients); i++) {
      RENDER_TEXT({.text = TextFormat("- Addr: %u - %s", server->clients[i].address, server->clients[i].name)});
    }
    RENDER_BUTTON({.message = "Create save", .on_click_func = button_click_simple(on_click), .text_y_offset = -4});
  }
  UI_GROUP_DESTROY();
}
