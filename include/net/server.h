#pragma once

#include "../game.h"
//#include <pthread.h>
#include "queue.h"
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
  int32_t player_id;
  char *name;
  addr_t address;
} Client;

typedef struct _server_game {
  addr_t server_addr;
  Game game;
  SaveDescriptor server_save;
  Client *clients;
  PacketQueue packet_queue;
  Bump packet_bump;

  PacketLogger packet_logger;
} ServerGame;

extern ServerGame SERVER_GAME;

// Mutex needs to be locked
Client *server_client_by_id(ServerGame *game, int32_t player_id);

void server_start(const char *ip_addr, uint32_t port);

void server_init(ServerGame *game);

void server_deinit(ServerGame *game);

void server_tick(ServerGame *game);
