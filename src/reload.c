#include "../include/reload.h"

#define CLIENT_RELOAD(client_game_ptr, src_file_prefix)                                                                                    \
  extern void src_file_prefix##_on_reload(ClientGame *game);                                                                               \
  src_file_prefix##_on_reload(client_game_ptr);

#define COMMON_RELOAD(game_ptr, src_file_prefix)                                                                                           \
  extern void src_file_prefix##_on_reload(Game *game);                                                                                     \
  src_file_prefix##_on_reload(game_ptr)

void client_reload(ClientGame *client) {
  if (!client->initializing) {
    assets_unload(&client->asset_manager);
  }

  assets_load(&client->asset_manager);

  CLIENT_RELOAD(client, tile);
  CLIENT_RELOAD(client, keybinds);
  // TODO: Move to asset manager
  CLIENT_RELOAD(client, shaders);
  CLIENT_RELOAD(client, world);
}

void common_reload(Game *game) {
  COMMON_RELOAD(game, config);
  COMMON_RELOAD(game, save_names);
}
