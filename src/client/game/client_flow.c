#include "../../../include/net/client.h"
#include "../../../include/game.h"

void client_init_loaded_save(ClientGame *client) {
  client->world = client->game->client_world;
  client->player = client->game->client_player;
}
