#pragma once

#include "../bytebuf.h"
#include "../game.h"
#include <stddef.h>
#include "lilc/numbers.h"
#include <stdbool.h>

typedef enum {
  PACKET_ERROR,
  S2C_PLAYER_JOIN,
  S2C_SYNC_SPACE,
  S2C_CLIENT_ACCEPTED,
  C2S_CLIENT_CONNECT,
  S2C_CLIENT_CONNECTED,
  C2S_CLIENT_DISCONNECT,

  _amount_packet_ids,
} PacketId;

typedef struct {
  PacketId id;
  void *payload;
} Packet;

typedef void (*PacketEncodeFunc)(const Packet *, ByteBuf *, DataContext);

typedef void (*PacketDecodeFunc)(Packet *, ByteBuf *buf, DataContext);

typedef void (*PacketHandleFunc)(const Packet *);

typedef struct {
  PacketEncodeFunc encode_func;
  PacketDecodeFunc decode_func;
  PacketHandleFunc handle_func;
  size_t payload_size;
} PacketInfo;

extern PacketInfo PACKET_INFOS[_amount_packet_ids];

void packets_setup(void);

void packet_send(i32 addr, PacketId id, void *payload);

void packet_receive(i32 addr, Packet *packet);

typedef struct {
  FILE *log_file;
} PacketLogger;

void packet_log(PacketLogger *logger, const Packet *packet, GameSide target);

void packet_alloc(Packet *packet);
