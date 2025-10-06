#pragma once

#include <stdbool.h>
#include "../space.h"
#include "packets.h"

typedef struct{
    enum {
        // Returned if decoding/encoding failed
        PACKET_ERROR,
        // Send to the player that just joined
        PACKET_S2C_PLAYER_JOIN,
        // Send to all players
        PACKET_S2C_NEW_PLAYER_JOINED,
        PACKET_S2C_SYNC_SPACE,
    } type;
    union {
        PacketS2CPlayerJoin s2c_player_join;
        PacketS2CNewPlayerJoined s2c_new_player_joined;
        PacketS2CSyncSpace s2c_sync_space;
    } var;
} Packet;

void packet_send(int addr, Packet packet, bool is_client);

Packet packet_receive(int addr, bool is_client);

void packet_handle(Packet *packet, struct _game *game);
