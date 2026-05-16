#include "../include/reload.h"
#include "../include/registry.h"

#define CLIENT_RELOAD(client_game_ptr, src_file_prefix)                                                                                    \
  extern void src_file_prefix##_on_reload(ClientGame *game);                                                                               \
  src_file_prefix##_on_reload(client_game_ptr);

#define COMMON_RELOAD(game_ptr, src_file_prefix)                                                                                           \
  extern void src_file_prefix##_on_reload(Game *game);                                                                                     \
  src_file_prefix##_on_reload(game_ptr)

void client_reload(ClientGame *client) {
  if (!client->initializing) {
    assets_unload(&client->asset_manager);
    animation_manager_unload(&client->tex_manager, &client->asset_manager);
  }

  assets_load(&client->asset_manager);
  animation_manager_load(&client->tex_manager, &client->asset_manager);

  tiles_reload(client);
  keybinds_reload(client);

  world_on_reload(client);
}

void common_reload(Game *game) {
  config_reload(game);
  save_names_reload(game);

#ifdef RELOADABLE
  system("gurd recompile-dynamic");
#endif

  if (!game->initializing) {
    registries_unload(&game->registries);
    tile_tex_manager_unload(&CLIENT_GAME.tile_tex_manager, &game->registries, &CLIENT_GAME.asset_manager);
  }

  registries_load(&game->registries);
  if (GAME_SIDE == SIDE_CLIENT) {
    tile_tex_manager_load(&CLIENT_GAME.tile_tex_manager, &game->registries, &CLIENT_GAME.asset_manager);
  }
}
