#pragma once

#include "../../include/registries/menus.h"
#include "../../include/net/client.h"

/* Render functions */

void backpack_menu_render(UiRenderer *renderer, ClientGame *client);

void debug_menu_render(UiRenderer *renderer, ClientGame *client);

void dialog_menu_render(UiRenderer *renderer, ClientGame *game);

void host_menu_render(UiRenderer *renderer, ClientGame *game);

void inventory_menu_render(UiRenderer *renderer, ClientGame *game);

void load_save_menu_render(UiRenderer *renderer, ClientGame *game);

void map_menu_render(UiRenderer *renderer, ClientGame *game);

void multiplayer_menu_render(UiRenderer *renderer, ClientGame *game);

void new_save_menu_render(UiRenderer *renderer, ClientGame *game);

void save_menu_render(struct _ui_renderer *renderer, struct _client_game *game);

void start_menu_render(UiRenderer *renderer, ClientGame *game);

/* Open functions */

void dialog_menu_open(ClientGame *client);

void host_menu_open(ClientGame *client);

void new_save_menu_open(ClientGame *game);
