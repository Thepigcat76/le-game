#pragma once

#include "client.h"

typedef struct {
  i32 player_id;
  Player player;
} PayloadPlayerJoin;

typedef struct {
  Space space;
} PayloadSyncSpace;

typedef struct {
  i32 player_id;
} PayloadClientAccepted;

typedef struct {
  i32 player_id;
  char *client_name;
} PayloadClientConnect;

typedef struct {
  i32 new_player_id;
  char *client_name;
} PayloadClientConnected;

typedef struct {
  i32 player_id;
} PayloadClientDisconnect;
