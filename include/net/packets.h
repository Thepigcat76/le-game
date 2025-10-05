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