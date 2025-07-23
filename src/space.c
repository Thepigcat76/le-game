#include "../include/space.h"
#include "../include/array.h"
#include "../include/data/data_reader.h"

SpaceType *SPACES = NULL;

void space_types_init(void) {
  SPACES = array_new_capacity(SpaceType, 16, &HEAP_ALLOCATOR);
  // TODO: Maybe do caching for base world as well, idk... probably makes sense
  SPACES[SPACE_BASE] =
      (SpaceType){.space_id = SPACE_BASE, .default_space = true, .cached = false, .global = true, .world_type = &WORLD_TYPES[WORLD_BASE]};
  SPACES[SPACE_DUNGEON_TEST] = (SpaceType){.space_id = SPACE_DUNGEON_TEST,
                                           .default_space = false,
                                           .cached = false,
                                           .global = false,
                                           .world_type = &WORLD_TYPES[WORLD_DUNGEON_TEST]};
}

void space_create(SpaceDescriptor desc, float seed, Space *space) {
  space->desc = desc;
  space->world = world_new(desc.type->world_type, space->seed);
  world_gen(&space->world);
}

void space_create_default(float seed, Space *space) { space_create((SpaceDescriptor){.type = &SPACES[SPACE_BASE], .id = 0}, seed, space); }

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
  case SPACE_DUNGEON_TEST:
    return "dungeon_test";
  }
}

SpaceId space_id_by_name(const char *name) {
  if (str_eq(name, "base")) {
    return SPACE_BASE;
  }
  if (str_eq(name, "dungeon_test")) {
    return SPACE_DUNGEON_TEST;
  }
  PANIC_FMT("Invalid space id: %s", name);
}
