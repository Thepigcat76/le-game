#include "../../../include/net/client.h"

void client_init_loaded_save(ClientGame *client, Save *save) {
  client->world = &save->loaded_spaces[0].world;
}
