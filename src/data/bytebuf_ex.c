#include "../../include/data/bytebuf_ex.h"
#include "../../include/data/data_reader.h"
#include "../../include/data/load.h"
#include "../../include/data/save.h"
#include "../../include/net/client.h"

void space_encode(const Space *space, ByteBuf *buf, DataContext ctx) {
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
  world_save(world, &map, ctx);
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

void space_decode(Space *space, ByteBuf *buf, DataContext ctx) {
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
  space_init(space, *desc, seed);
  space->seed = seed;

  printf("Reader index, before world: %zu", buf->reader_index);
  DataMap map = byte_buf_read_data(buf).var.data_map;
  Data data = data_map(map);
  char *map_str = data_reader_read_data(&data);
  FILE *f = fopen("server-save-world-decoded", "w");
  fputs(map_str, f);
  fclose(f);

  world_load(&space->world, &map, ctx);
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
