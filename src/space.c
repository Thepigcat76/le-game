#include "../include/space.h"
#include "../include/data/data_reader.h"

const SpaceDescriptor SPACE_DESC_DEFAULT = {.space_id = SPACE_BASE, .lazy_loaded = false};

void space_create(SpaceDescriptor desc, Space *space) {
  space->desc = desc;
  switch (desc.space_id) {
  case SPACE_BASE: {
    space->world = world_new();
    break;
  }
  }
}

void space_create_default(Space *space) { space_create(SPACE_DESC_DEFAULT, space); }

// Load/Save

#define SPACE_BYTE_BUF_SIZE sizeof(Chunk) * WORLD_LOADED_CHUNKS + 100 + sizeof(BeingInstance) * 200

void space_load(SaveDescriptor save_desc, SpaceDescriptor space_desc, Space *space) {
  LOAD_SPACE_DATA(save_desc, space_desc, "world", SPACE_BYTE_BUF_SIZE, byte_buf, {
    Data data_map = byte_buf_read_data(&byte_buf);
    DataMap *world_map = &data_map.var.data_map;
    world_load(&space->world, world_map);

    data_free(&data_map);
  });
}

void space_save(SaveDescriptor save_desc, const Space *space) {
  SAVE_SPACE_DATA(save_desc, space->desc, "world", SPACE_BYTE_BUF_SIZE, byte_buf, {
    DataMap world_map = data_map_new(2000);
    world_save(&space->world, &world_map);

    Data world_data = data_map(world_map);
    byte_buf_write_data(&byte_buf, &world_data);
    
    char *str = data_reader_read_data(&world_data);
    free(str);

    data_free(&world_data);
  });
}

// String functions

char *space_id_to_name(SpaceId id) {
  switch (id) {
  case SPACE_BASE:
    return "base";
  }
}

SpaceId space_id_by_name(const char *name) {
  if (strcmp(name, "base") == 0) {
    return SPACE_BASE;
  }
  PANIC_FMT("Invalid space id: %s", name);
}
