#pragma once

#include <stdbool.h>
#include "../space.h"
#include "../player.h"

#define PACKET_S2C(name) PacketS2C##name

#define PACKET_C2S(name) PacketC2S##name

// Send all players
typedef struct {
  int player_id;
  Player player;
} PACKET_S2C(PlayerJoin);

// Sync a space to a player
typedef struct {
  Space space;
} PACKET_S2C(SyncSpace);

// Tell client that it was accepted
typedef struct {
  int player_id;
} PACKET_S2C(ClientAccepted);

// Send connection relevant data to server
typedef struct {
  int player_id;
  char *client_name;
} PACKET_C2S(ClientConnect);

// Send connection relevant data from server to all clients, besides the current one
typedef struct {
  char *client_name;
  int new_player_id;
} PACKET_S2C(ClientConnected);

// Tell server that client disconnected
typedef struct {
  int player_id;
} PACKET_C2S(ClientDisconnect);

#define _PACKET_INTERNAL(_type, ...) ((Packet){.type = _type, .var = {__VA_ARGS__}})

#define PACKET_S2C_PLAYER_JOIN_NEW(...) _PACKET_INTERNAL(PACKET_S2C_PLAYER_JOIN, .s2c_player_join = (PacketS2CPlayerJoin) __VA_ARGS__)
#define PACKET_C2S_CLIENT_CONNECT_NEW(...) _PACKET_INTERNAL(PACKET_C2S_CLIENT_CONNECT, .c2s_client_connect = (PacketC2SClientConnect) __VA_ARGS__)
#define PACKET_C2S_CLIENT_DISCONNECT_NEW(...) _PACKET_INTERNAL(PACKET_C2S_CLIENT_DISCONNECT, .c2s_client_disconnect = (PacketC2SClientDisconnect) __VA_ARGS__)
#define PACKET_S2C_CLIENT_ACCEPTED_NEW(...) _PACKET_INTERNAL(PACKET_S2C_CLIENT_ACCEPTED, .s2c_client_accepted = (PacketS2CClientAccepted) __VA_ARGS__)
#define PACKET_S2C_CLIENT_CONNECTED_NEW(...) _PACKET_INTERNAL(PACKET_S2C_CLIENT_CONNECTED, .s2c_client_connected = (PacketS2CClientConnected) __VA_ARGS__)
#define PACKET_S2C_SYNC_SPACE_NEW(...) _PACKET_INTERNAL(PACKET_S2C_SYNC_SPACE, .s2c_sync_space = (PacketS2CSyncSpace) __VA_ARGS__)

typedef struct{
    enum {
        // Returned if decoding/encoding failed
        PACKET_ERROR,
        // Send to the player that just joined
        PACKET_S2C_SYNC_SPACE,
        // Send to all players
        PACKET_S2C_PLAYER_JOIN,
        PACKET_C2S_CLIENT_CONNECT,
        PACKET_C2S_CLIENT_DISCONNECT,
        PACKET_S2C_CLIENT_CONNECTED,
        PACKET_S2C_CLIENT_ACCEPTED,
    } type;
    union {
        PacketS2CPlayerJoin s2c_player_join;
        PacketS2CSyncSpace s2c_sync_space;
        PacketC2SClientConnect c2s_client_connect;
        PacketC2SClientDisconnect c2s_client_disconnect;
        PacketS2CClientConnected s2c_client_connected;
        PacketS2CClientAccepted s2c_client_accepted;
    } var;
} Packet;

void packet_send(int addr, Packet packet, bool is_client);

Packet packet_receive(int addr, bool is_client);

void packet_handle(Packet *packet, struct _game *game);
