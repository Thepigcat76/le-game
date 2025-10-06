#include "../../include/net/sockets.h"
#include <complex.h>
#include <stdlib.h>
#ifdef TARGET_WIN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif
#include "../../include/array.h"
#include "../../include/bytebuf.h"
#include "../../include/data/data_reader.h"
#include "../../include/game.h"
#include "../../include/net/packet.h"
#include <stdio.h>

static void byte_buf_send(int addr, ByteBuf buf) { sockets_send(addr, (SocketDataBuffer){buf.bytes, buf.writer_index}, 0); }

static void packet_fmt(Packet packet, int addr, bool serverbound, bool is_client, char *buf) {
  char *prefix = ">>>";
  char postfix[16];
  if (serverbound && is_client) {
    prefix = "<<<";
    sprintf(postfix, "TO %d", addr);
  } else if (!serverbound && is_client) {
    prefix = ">>>";
    sprintf(postfix, "FROM %d", addr);
  } else if (serverbound && !is_client) {
    prefix = ">>>";
    sprintf(postfix, "FROM %d", addr);
  } else if (!serverbound && !is_client) {
    prefix = "<<<";
    sprintf(postfix, "TO %d", addr);
  }

  switch (packet.type) {
  case PACKET_ERROR: {
    sprintf(buf, "%s [PACKET_ERROR] %s", prefix, postfix);
    break;
  }
  case PACKET_S2C_PLAYER_JOIN: {
    sprintf(buf, "%s [PLAYER_JOIN]{player=%d} %s", prefix, packet.var.s2c_player_join.player_id, postfix);
    break;
  }
  case PACKET_S2C_NEW_PLAYER_JOINED: {
    sprintf(buf, "%s [NEW_PLAYER_JOINED]{player=%d} %s", prefix, packet.var.s2c_new_player_joined.new_player_id, postfix);
    break;
  }
  case PACKET_S2C_SYNC_SPACE: {
    PacketS2CSyncSpace *ss_packet = &packet.var.s2c_sync_space;
    sprintf(buf, "%s [SYNC_SPACE]{space_world_seed=%f,save_name=%s} %s", prefix, ss_packet->space.seed,
            ss_packet->space.world.save_desc != NULL ? ss_packet->space.world.save_desc->config.save_name : "No save desc provided",
            postfix);
    break;
  }
  }
}

static void space_encode(const Space *space, ByteBuf *buf) {
  // Desc
  SpaceDescriptor desc = space->desc;
  {
    // Type
    byte_buf_write_byte(buf, desc.type->space_id);
    // Id
    byte_buf_write_int(buf, desc.id);
    // Loaded from disk
    byte_buf_write_byte(buf, desc.external);
  }
  // Seed
  char seed[64];
  sprintf(seed, "%f", space->seed);
  byte_buf_write_string(buf, seed);

  printf("Writer index, before world: %zu", buf->writer_index);
  // World
  const World *world = &space->world;
  DataMap map = data_map_new(2000);
  world_save(world, &map);
  Data data = data_map(map);
  char *data_string = data_reader_read_data(&data);
  FILE *f = fopen("space_packet_world.json", "w");
  fputs(data_string, f);
  fclose(f);
  byte_buf_write_data(buf, &data);
  // Initialized
  byte_buf_write_byte(buf, world->initialized);
  // Save desc
  const SaveDescriptor *save_desc = world->save_desc;
  // Has save desc
  bool has_save_desc = save_desc != NULL;
  byte_buf_write_byte(buf, has_save_desc);
  if (has_save_desc) {
    printf("Save desc for encoding: %p\n", save_desc);
    byte_buf_write_int(buf, save_desc->id);
    byte_buf_write_byte(buf, save_desc->is_server_save);
    // Save config
    SaveConfig config = save_desc->config;
    {
      // Save name
      byte_buf_write_string(buf, config.save_name);
    }
  }
}

static void space_decode(Space *space, ByteBuf *buf) {
  // Desc
  SpaceDescriptor *desc = &space->desc;
  {
    desc->type = &SPACES[byte_buf_read_byte(buf)];
    desc->id = byte_buf_read_int(buf);
    desc->external = byte_buf_read_byte(buf);
  }

  // Seed
  int len = byte_buf_read_int(buf);
  char *seed_buf = malloc(len);
  byte_buf_read_string(buf, seed_buf, len);

  float seed = atof(seed_buf);
  printf("Seed: %f, space: %p, desc: %p\n", seed, space, desc);
  space_create(*desc, seed, space);
  space->seed = seed;

  printf("Reader index, before world: %zu", buf->reader_index);
  DataMap map = byte_buf_read_data(buf).var.data_map;
  Data data = data_map(map);
  char *map_str = data_reader_read_data(&data);
  FILE *f = fopen("server-save-world-decoded", "w");
  fputs(map_str, f);
  fclose(f);

  world_load(&space->world, &map);
  // Initialized
  space->world.initialized = byte_buf_read_byte(buf);
  space->world.seed = seed;
  if (GAME_SIDE == SIDE_CLIENT) {
    world_initialize(&space->world);
  }

  // Save desc
  bool has_save_desc = byte_buf_read_byte(buf);
  if (has_save_desc) {
    SaveDescriptor save_desc;
    {
      save_desc.id = byte_buf_read_int(buf);
      save_desc.is_server_save = byte_buf_read_byte(buf);
      // Save config
      {
        // Save name
        size_t len = byte_buf_read_int(buf);
        char *save_name = malloc(len);
        byte_buf_read_string(buf, save_name, len);
        save_desc.config.save_name = save_name;
      }
    }
    SaveDescriptor *save_desc_clone = malloc(sizeof(SaveDescriptor));
    memcpy(save_desc_clone, &save_desc, sizeof(SaveDescriptor));
    space->world.save_desc = save_desc_clone;
  }
}

static void packet_encode(Packet packet, ByteBuf *buf) {
  byte_buf_write_byte(buf, packet.type);
  switch (packet.type) {
  case PACKET_ERROR: {
    break;
  }
  case PACKET_S2C_PLAYER_JOIN: {
    int player_id = packet.var.s2c_player_join.player_id;
    byte_buf_write_byte(buf, player_id);
    break;
  }
  case PACKET_S2C_NEW_PLAYER_JOINED: {
    int player_id = packet.var.s2c_new_player_joined.new_player_id;
    byte_buf_write_byte(buf, player_id);
    break;
  }
  case PACKET_S2C_SYNC_SPACE: {
    Space space = packet.var.s2c_sync_space.space;
    space.desc.external = true;
    space_encode(&space, buf);
    break;
  }
  }
}

static Packet packet_decode(ByteBuf *buf) {
  int type = byte_buf_read_byte(buf);
  switch (type) {
  case PACKET_S2C_PLAYER_JOIN: {
    int player_id = byte_buf_read_byte(buf);
    return (Packet){.type = type, .var = {.s2c_player_join = {.player_id = player_id}}};
  }
  case PACKET_S2C_NEW_PLAYER_JOINED: {
    int player_id = byte_buf_read_byte(buf);
    return (Packet){.type = type, .var = {.s2c_new_player_joined = {.new_player_id = player_id}}};
  }
  case PACKET_S2C_SYNC_SPACE: {
    Space space;
    space_decode(&space, buf);
    return (Packet){.type = type, .var = {.s2c_sync_space = {.space = space}}};
  }
  default: {
    printf("ERROR DECODING\n");
    return (Packet){.type = PACKET_ERROR};
  }
  }
}

static void handle_space_sync(PacketS2CSyncSpace *packet, Game *game) {
    world_initialize(&packet->space.world);
    // Create save
    Save save = save_new((SaveDescriptor){.id = 0, .is_server_save = true, .config = {.seed = packet->space.seed, .save_name = "Server-Save"}});
    array_add(save.loaded_spaces, packet->space);
    // Assign save to cur_save
    game->cur_save = save;
    // Create player
    Player player = player_new(game);
    array_add(game->cur_save.players, player);
    
    game->client_world = &game->cur_save.loaded_spaces[0].world;
    game->client_player = &game->cur_save.players[0];
    client_init_loaded_save(&CLIENT_GAME, &game->cur_save);
    client_set_menu(&CLIENT_GAME, MENU_NONE);
    CLIENT_GAME.game->save_loaded = true;
    CLIENT_GAME.paused = false;
}

void packet_handle(Packet *packet, Game *game) {
  switch (packet->type) {
  case PACKET_ERROR: {
    break;
  }
  case PACKET_S2C_PLAYER_JOIN: {
    printf("Welcome from the server\n");
    break;
  }
  case PACKET_S2C_NEW_PLAYER_JOINED: {
    break;
  }
  case PACKET_S2C_SYNC_SPACE: {
    handle_space_sync(&packet->var.s2c_sync_space, game);
    break;
  }
  }
}

void packet_send(int addr, Packet packet, bool is_client) {
  uint8_t bytes[128000];
  ByteBuf buf = {.writer_index = 0, .reader_index = 0, .capacity = 128000, .bytes = bytes};
  packet_encode(packet, &buf);
  // Send: 2-byte length + data
  uint16_t len = buf.writer_index;
  uint8_t header[2] = {len >> 8, len & 0xFF};
  sockets_send(addr, (SocketDataBuffer){header, 2}, 0);
  sockets_send(addr, (SocketDataBuffer){buf.bytes, len}, 0);
  char print_buf[256];
  packet_fmt(packet, addr, is_client, is_client, print_buf);
  printf("%s\n", print_buf);
}

Packet packet_receive(int addr, bool is_client) {
  uint8_t len_buf[2];
  ssize_t n = sockets_receive(addr, (SocketDataBuffer){len_buf, 2}, MSG_WAITALL);
  if (n != 2) {
    perror("Failed to read length");
    return (Packet){.type = PACKET_ERROR};
  }

  uint16_t len = (len_buf[0] << 8) | len_buf[1];
  if (len > 16000) {
    fprintf(stderr, "Packet too long: %u\n", len);
    return (Packet){.type = PACKET_ERROR};
  }

  uint8_t bytes[16000];
  ByteBuf buf = {.reader_index = 0, .writer_index = len, .capacity = 16000, .bytes = bytes};
  n = sockets_receive(addr, (SocketDataBuffer){buf.bytes, len}, MSG_WAITALL);
  if (n != len) {
    perror("Failed to read full packet");
    return (Packet){.type = PACKET_ERROR};
  }

  buf.writer_index = 0;

  Packet packet = packet_decode(&buf);

  char print_buf[256];
  packet_fmt(packet, addr, !is_client, is_client, print_buf);
  printf("%s\n", print_buf);

  return packet;
}