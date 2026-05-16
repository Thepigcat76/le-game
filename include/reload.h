#pragma once

#include "net/client.h"

void client_reload(ClientGame *client);

void common_reload(Game *game);

/* RELOAD-FUNCTIONS */

/* CLIENT */

void keybinds_reload(ClientGame *client);

/* REACTIONARY-FUNCTIONS */

void world_on_reload(ClientGame *client);

/* SERVER */

void config_reload(Game *game);

void save_names_reload(Game *game);
