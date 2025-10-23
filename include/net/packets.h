#pragma once

#include "../space.h"

// Sent to all players
typedef struct {
  // Id of the new player that joined
  int new_player_id;
} PacketS2CNewPlayerJoined;

// Send to player who just joined
typedef struct {
  int player_id;
} PacketS2CPlayerJoin;

// Sync a space to a player
typedef struct {
  Space space;
} PacketS2CSyncSpace;

// Send connection relevant data to server
typedef struct {
  char *client_name;
} PacketC2SClientConnect;

// Send connection relevant data from server to all clients, besides the current one
typedef struct {
  char *client_name;
  int new_player_id;
} PacketS2CClientConnected;
