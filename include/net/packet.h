#pragma once

#include <stdbool.h>

typedef struct{
    enum {
        // Returned if decoding/encoding failed
        PACKET_ERROR,
        // Send to the player that just joined
        PACKET_S2C_PLAYER_JOIN,
        // Send to all players
        PACKET_S2C_NEW_PLAYER_JOINED,
    } type;
    union {
        struct {
            int player_id;
        } s2c_player_join;
        struct {
            int player_id;
        } s2c_new_player_joined;
    } var;
} Packet;

void packet_send(int addr, Packet packet, bool is_client);

Packet packet_receive(int addr, bool is_client);
