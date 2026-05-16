#include "../../include/net/packet.h"
#include "../../include/data/bytebuf_ex.h"
#include "../../include/data/load.h"
#include "../../include/data/save.h"
#include "../../include/net/client.h"
#include "../../include/net/payloads.h"
#include "../../include/net/server.h"
#include "../../include/net/sockets.h"
#include "../../include/netincludes.h"
#include "../../include/player.h"
#include "../../include/space.h"
#include "lilc/log.h"
#include <lilc/alloc.h>
#include <lilc/bump.h>
#include <lilc/dynstr.h>
#include <stdio.h>

PacketInfo PACKET_INFOS[_amount_packet_ids];

static void packet_add(PacketId id, PacketEncodeFunc encode_func, PacketDecodeFunc decode_func, PacketHandleFunc handle_func,
                       size_t payload_size);

/* PACKET IMPLEMENTATIONS */

/* PLAYER-JOIN */

static void packet_player_join_encode(const Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadPlayerJoin *payload = packet->payload;

  i32 player_id = payload->player_id;
  byte_buf_write_int(buf, player_id);

  DataMap player_map = data_map_new(200);
  player_save(&payload->player, &player_map, (DataContext){.registries = ctx.registries});
  Data player_data = data_map(player_map);
  byte_buf_write_data(buf, &player_data);
}

static void packet_player_join_decode(Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadPlayerJoin *payload = packet->payload;

  payload->player_id = byte_buf_read_int(buf);

  DataMap player_map = byte_buf_read_data(buf).var.data_map;
  player_init(&payload->player);
  player_load(&payload->player, &player_map, (DataContext){.registries = ctx.registries});
}

static void packet_player_join_handle(const Packet *packet) {
  PayloadPlayerJoin *payload = packet->payload;

  if (payload->player_id != CLIENT_GAME.player_id) {
    Player *player = &payload->player;
    // TODO: Maybe we only need to sync RenderDesc?
    PlayerRenderDescriptor desc = {.animation_frame = player->animation_frame,
                                   .frame_timer = player->frame_timer,
                                   .box = player->box,
                                   .direction = player->direction,
                                   .in_water = player->in_water,
                                   .walking = player->walking};
    array_add(CLIENT_GAME.players, desc);
  } else {
    log_info("Welcome from the server");
  }
}

/* SYNC-SPACE */

static void packet_sync_space_encode(const Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadSyncSpace *payload = packet->payload;

  Space space = payload->space;
  space.desc.external = true;
  space_encode(&space, buf, ctx);
}

static void packet_sync_space_decode(Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadSyncSpace *payload = packet->payload;

  space_decode(&payload->space, buf, ctx);
}

static void packet_sync_space_handle(const Packet *packet) {
  PayloadSyncSpace *payload = packet->payload;

  world_initialize(&payload->space.world);
  // Create save
  Save save = save_new((SaveDescriptor){
      .id = 0,
      .is_server_save = true,
      .config = {.seed = payload->space.seed, .save_name = "Server-Save"},
  });
  array_add(save.loaded_spaces, payload->space);
  // Assign save to cur_save
  CLIENT_GAME.game.cur_save = save;
  // Create player
  Player player = {0};
  player_init(&player);
  // array_add(game->cur_save.players, player);

  // FIXME: Dangerous, since mem location of first element might change
  // game->client_player = &game->cur_save.players[0];
  client_init_loaded_save(&CLIENT_GAME, &CLIENT_GAME.game.cur_save);
  client_set_menu(&CLIENT_GAME, MENU_NONE);
  CLIENT_GAME.game.save_loaded = true;
  CLIENT_GAME.state.paused = false;
}

/* CLIENT-ACCEPTED */

static void packet_client_accepted_encode(const Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadClientAccepted *payload = packet->payload;

  i32 player_id = payload->player_id;
  byte_buf_write_int(buf, player_id);
}

static void packet_client_accepted_decode(Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadClientAccepted *payload = packet->payload;

  payload->player_id = byte_buf_read_int(buf);
}

static void packet_client_accepted_handle(const Packet *packet) {
  PayloadClientAccepted *payload = packet->payload;

  CLIENT_GAME.player_id = payload->player_id;
  CLIENT_GAME.players = array_new_capacity(PlayerRenderDescriptor, 8, &HEAP_ALLOCATOR);
  log_info("Player accepted");
}

/* CLIENT-CONNECT */

static void packet_client_connect_encode(const Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadClientConnect *payload = packet->payload;

  i32 player_id = payload->player_id;
  byte_buf_write_int(buf, player_id);

  char *client_name = payload->client_name;
  byte_buf_write_string(buf, client_name);
}

static void packet_client_connect_decode(Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadClientConnect *payload = packet->payload;

  payload->player_id = byte_buf_read_int(buf);

  size_t str_len = byte_buf_read_int(buf);

  payload->client_name = heap_alloc(str_len + 1);
  byte_buf_read_string(buf, payload->client_name, str_len);
}

static void packet_client_connect_handle(const Packet *packet) {
  PayloadClientConnect *payload = packet->payload;

  log_debug("Client connect data");
  for (size_t i = 0; i < array_len(SERVER_GAME.clients); i++) {
    if (SERVER_GAME.clients[i].player_id == payload->player_id) {
      SERVER_GAME.clients[i].name = payload->client_name;
      break;
    }
  }
  Client *client = server_client_by_id(&SERVER_GAME, payload->player_id);
  client->name = payload->client_name;

  size_t clients = array_len(SERVER_GAME.clients);
  for (size_t i = 0; i < clients; i++) {
    PayloadClientConnected client_connected_payload = {.client_name = payload->client_name, .new_player_id = payload->player_id};
    packet_send(SERVER_GAME.clients[i].address, S2C_CLIENT_CONNECTED, &client_connected_payload);
  }
}

/* CLIENT-CONNECTED */

static void packet_client_connected_encode(const Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadClientConnected *payload = packet->payload;

  i32 player_id = payload->new_player_id;
  byte_buf_write_int(buf, player_id);

  char *client_name = payload->client_name;
  byte_buf_write_string(buf, client_name);
}

static void packet_client_connected_decode(Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadClientConnected *payload = packet->payload;

  payload->new_player_id = byte_buf_read_int(buf);

  size_t str_len = byte_buf_read_int(buf);

  payload->client_name = heap_alloc(str_len + 1);
  byte_buf_read_string(buf, payload->client_name, str_len);
}

static void packet_client_connected_handle(const Packet *packet) {
  PayloadClientConnected *payload = packet->payload;
  log_info("New client connected! Name: %s, Id: %d", payload->client_name, payload->new_player_id);
}

/* CLIENT-DISCONNECT */

static void packet_client_disconnect_encode(const Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadClientDisconnect *payload = packet->payload;

  i32 player_id = payload->player_id;
  byte_buf_write_int(buf, player_id);
}

static void packet_client_disconnect_decode(Packet *packet, ByteBuf *buf, DataContext ctx) {
  PayloadClientDisconnect *payload = packet->payload;

  payload->player_id = byte_buf_read_int(buf);
}

static void packet_client_disconnect_handle(const Packet *packet) {
  PayloadClientDisconnect *payload = packet->payload;

  i32 player_id = payload->player_id;

  size_t i;
  for (i = 0; i < array_len(SERVER_GAME.clients); i++) {
    if (SERVER_GAME.clients[i].player_id == player_id) {
      break;
    }
  }

  array_remove(SERVER_GAME.clients, i);
}

/* PACKET REGISTRATION */

void packets_setup(void) {
  packet_add(PACKET_ERROR, NULL, NULL, NULL, 0);

  packet_add(S2C_PLAYER_JOIN, packet_player_join_encode, packet_player_join_decode, packet_player_join_handle, sizeof(PayloadPlayerJoin));
  packet_add(S2C_SYNC_SPACE, packet_sync_space_encode, packet_sync_space_decode, packet_sync_space_handle, sizeof(PayloadSyncSpace));
  packet_add(S2C_CLIENT_ACCEPTED, packet_client_accepted_encode, packet_client_accepted_decode, packet_client_accepted_handle,
             sizeof(PayloadClientAccepted));
  packet_add(C2S_CLIENT_CONNECT, packet_client_connect_encode, packet_client_connect_decode, packet_client_connect_handle,
             sizeof(PayloadClientConnect));
  packet_add(S2C_CLIENT_CONNECTED, packet_client_connected_encode, packet_client_connected_decode, packet_client_connected_handle,
             sizeof(PayloadClientConnected));
  packet_add(C2S_CLIENT_DISCONNECT, packet_client_disconnect_encode, packet_client_disconnect_decode, packet_client_disconnect_handle,
             sizeof(PayloadClientDisconnect));
}

static void packet_add(PacketId id, PacketEncodeFunc encode_func, PacketDecodeFunc decode_func, PacketHandleFunc handle_func,
                       size_t payload_size) {
  PACKET_INFOS[id] = (PacketInfo){
      .encode_func = encode_func,
      .decode_func = decode_func,
      .handle_func = handle_func,
      .payload_size = payload_size,
  };
}

void packet_send(i32 addr, PacketId id, void *payload) {
  Packet packet = {.id = id, .payload = payload};
  PacketInfo info = PACKET_INFOS[id];

  packet_log(GAME_SIDE == SIDE_CLIENT ? &CLIENT_GAME.packet_logger : &SERVER_GAME.packet_logger, &packet, !GAME_SIDE);

  DataContext ctx;
  if (GAME_SIDE == SIDE_CLIENT) {
    ctx.registries = &CLIENT_GAME.game.registries;
  } else {
    ctx.registries = &SERVER_GAME.game.registries;
  }

  u8 bytes[128000];
  ByteBuf buf = {.writer_index = 0, .reader_index = 0, .capacity = 128000, .bytes = bytes};
  byte_buf_write_int(&buf, id);
  info.encode_func(&packet, &buf, ctx);
  // Send: 2-byte length + data
  u16 len = buf.writer_index;
  u8 header[2] = {len >> 8, len & 0xFF};
  sockets_send(addr, (SocketDataBuffer){header, 2}, 0);
  sockets_send(addr, (SocketDataBuffer){buf.bytes, len}, 0);

  // char print_buf[256];
  // packet_fmt(packet, addr, is_client, is_client, print_buf);
  // printf("%s\n", print_buf);
}

void packet_receive(i32 addr, Packet *packet) {
  u8 len_buf[2];
  ssize_t n = sockets_receive(addr, (SocketDataBuffer){len_buf, 2}, MSG_WAITALL);
  if (n != 2) {
    perror("Failed to read length");
    packet->id = PACKET_ERROR;
    return;
  }

  uint16_t len = (len_buf[0] << 8) | len_buf[1];
  if (len > 16000) {
    fprintf(stderr, "Packet too long: %u\n", len);
    packet->id = PACKET_ERROR;
    return;
  }

  uint8_t bytes[16000];
  ByteBuf buf = {.reader_index = 0, .writer_index = len, .capacity = 16000, .bytes = bytes};
  n = sockets_receive(addr, (SocketDataBuffer){buf.bytes, len}, MSG_WAITALL);
  if (n != len) {
    perror("Failed to read full packet");
    packet->id = PACKET_ERROR;
    return;
  }

  buf.writer_index = 0;

  PacketId id = byte_buf_read_int(&buf);
  packet->id = id;

  DataContext ctx;
  if (GAME_SIDE == SIDE_CLIENT) {
    ctx.registries = &CLIENT_GAME.game.registries;
  } else {
    ctx.registries = &SERVER_GAME.game.registries;
  }

  PacketInfo packet_info = PACKET_INFOS[id];
  packet_alloc(packet);
  packet_info.decode_func(packet, &buf, ctx);

  packet_log(GAME_SIDE == SIDE_CLIENT ? &CLIENT_GAME.packet_logger : &SERVER_GAME.packet_logger, packet, GAME_SIDE);

  // char print_buf[256];
  // packet_fmt(packet, addr, !is_client, is_client, print_buf);
  // printf("%s\n", print_buf);
}

dyn_string_t packet_fmt(const Packet *packet, Allocator *allocator) {
  dyn_string_t str = {0};
  dyn_string_init(&str, allocator);

  switch (packet->id) {
  case PACKET_ERROR: {
    dyn_string_printf(&str, "PACKET_ERROR");
  } break;
  case S2C_PLAYER_JOIN: {
    PayloadPlayerJoin *payload = packet->payload;
    dyn_string_printf(&str, "S2C PACKET_PLAYER_JOIN(player_id=%d)", payload->player_id);
  } break;
  case S2C_SYNC_SPACE: {
    PayloadSyncSpace *payload = packet->payload;
    float seed = payload->space.world.seed;
    dyn_string_printf(&str, "S2C PACKET_SYNC_SPACE(seed=%f)", seed);
  } break;
  case S2C_CLIENT_ACCEPTED: {
    PayloadClientAccepted *payload = packet->payload;
    dyn_string_printf(&str, "S2C PACKET_CLIENT_ACCEPTED(player_id=%d)", payload->player_id);
  } break;
  case C2S_CLIENT_CONNECT: {
    PayloadClientConnect *payload = packet->payload;
    dyn_string_printf(&str, "C2S PACKET_CLIENT_CONNECT(player_id=%d,player_name=%s)", payload->player_id, payload->client_name);
  } break;
  case S2C_CLIENT_CONNECTED: {
    PayloadClientConnected *payload = packet->payload;
    dyn_string_printf(&str, "S2C PACKET_CLIENT_CONNECTED(player_id=%d,client_name=%s)", payload->new_player_id, payload->client_name);
  } break;
  case C2S_CLIENT_DISCONNECT: {
    PayloadClientDisconnect *payload = packet->payload;
    dyn_string_printf(&str, "C2S PACKET_CLIENT_DISCONNECT(player_id=%d)", payload->player_id);
  } break;
  default: {
    dyn_string_printf(&str, "Unknow packet, id: %d", packet->id);
  } break;
  }

  return str;
}

void packet_log(PacketLogger *logger, const Packet *packet, GameSide target) {
  char *direction = target == GAME_SIDE ? ">>>" : "<<<";

  dyn_string_t str = packet_fmt(packet, &HEAP_ALLOCATOR);
  fprintf(logger->log_file, "%s %s\n", direction, str.string);
  fflush(logger->log_file);
  dyn_string_free(&str);
}

void packet_alloc(Packet *packet) {
  if (GAME_SIDE == SIDE_CLIENT) {
    log_debug("CLIENT ALLOC");
    packet->payload = bump_alloc(&CLIENT_CONNECTION.packet_bump, PACKET_INFOS[packet->id].payload_size);

    log_debug("Packet ptr after alloc: %p", packet->payload);
  } else {
    packet->payload = bump_alloc(&SERVER_GAME.packet_bump, PACKET_INFOS[packet->id].payload_size);
  }
}
