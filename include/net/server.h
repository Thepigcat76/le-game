#pragma once

//#include "../game.h"
//#include <pthread.h>
#include "sockets.h"
//
//typedef struct {
//  addr_t server_addr;
//  addr_t client_addresses[MAX_PLAYERS];
//  Game server_game;
//  pthread_mutex_t server_mutex;
//} Server;

typedef struct {
  
} ServerGame;

void server_start(const char *ip_addr, uint32_t port);
