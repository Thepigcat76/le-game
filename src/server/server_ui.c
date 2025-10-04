#include <raylib.h>
#define CTX_SERVER
#include "../../include/ui.h"
#undef CTX_SERVER
#include "../../include/net/server.h"
#include "../../include/save_names.h"
#include "../../include/server_ui.h"
#include "../../include/shared.h"
#include "../../include/save_desc.h"
#include "../../vendor/cJSON.h"

static void on_click(void) {
  printf("Button pressed\n");
  if (!DirectoryExists("server-save")) {
    dir_create("server-save");
  }
  SaveConfig config = {.save_name = save_names_random_name(), .seed = string_to_world_seed("")};
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
}

void server_ui_render(UiRenderer *server_ui_renderer, ServerGame *server) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 24,
      .scale = 1,
      .font_scale = 24,
  });

  RENDER_TEXT({.text = "Server"});
  RENDER_TEXT({.text = TextFormat("Connected clients: %zu", server->players)});
  for (size_t i = 0; i < server->players; i++) {
    RENDER_TEXT({.text = TextFormat("- Addr: %u", server->client_addresses[i])});
  }
  RENDER_BUTTON({.message = "Create save", .on_click_func = button_click_simple(on_click), .text_y_offset = -4});
}
