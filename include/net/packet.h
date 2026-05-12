#pragma once

#include "../bytebuf.h"
#include <stddef.h>
#include "lilc/numbers.h"

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

typedef void (*PacketEncodeFunc)(const Packet *, ByteBuf *buf);

typedef void (*PacketDecodeFunc)(Packet *, ByteBuf *buf);

typedef void (*PacketHandleFunc)(const Packet *);

typedef struct {
  PacketEncodeFunc encode_func;
  PacketDecodeFunc decode_func;
  PacketHandleFunc handle_func;
} PacketInfo;

extern PacketInfo PACKET_INFOS[_amount_packet_ids];

void packets_setup(void);

void packet_send(i32 addr, PacketId id, void *payload);

void packet_receive(i32 addr, Packet *packet);
