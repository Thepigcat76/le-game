#pragma once

//#include "../game.h"
//#include <pthread.h>
#include "sockets.h"
#include "../save.h"
//
//typedef struct {
//  addr_t server_addr;
//  addr_t client_addresses[MAX_PLAYERS];
//  Game server_game;
//  pthread_mutex_t server_mutex;
//} Server;

typedef struct {
  addr_t server_addr;
  struct _game *game;
  SaveDescriptor server_save;
  size_t clients_amount;
  addr_t client_addresses[MAX_CLIENTS];
} ServerGame;

extern ServerGame SERVER_GAME;

void server_start(const char *ip_addr, uint32_t port);

void server_init(ServerGame *game);

void server_deinit(ServerGame *game);

void server_tick(ServerGame *game);
