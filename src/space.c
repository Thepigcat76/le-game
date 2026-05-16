#include "../include/space.h"
#include "lilc/array.h"
#include "../include/data/data_reader.h"
#include "../include/data/load.h"
#include "../include/data/save.h"

SpaceType *SPACES = NULL;

void space_types_init(void) {
  SPACES = array_new_capacity(SpaceType, 16, &HEAP_ALLOCATOR);
  // FIXME: MOVE TO NEW REGISTRY
  // TODO: Maybe do caching for base world as well, idk... probably makes sense
  //SPACES[SPACE_BASE] =
      //(SpaceType){.space_id = SPACE_BASE, .default_space = true, .cached = false, .global = true, .world_type = &WORLD_TYPES[WORLD_BASE]};
  //SPACES[SPACE_DUNGEON_TEST] = (SpaceType){.space_id = SPACE_DUNGEON_TEST,
  //                                         .default_space = false,
  //                                         .cached = false,
  //                                         .global = false,
  //                                         .world_type = &WORLD_TYPES[WORLD_DUNGEON_TEST]};
}

void space_init(Space *space, SpaceDescriptor desc, float seed) {
  space->desc = desc;
  printf("Creating space - Seed: %f, type: %p\n", seed, desc.type);

  // FIXME: Use actual world id again
  world_init(&space->world, WORLD_BASE, seed);
  if (!desc.external) {
    world_gen(&space->world);
  }
}

void space_init_default(Space *space, float seed) { space_init(space, (SpaceDescriptor){.type = &SPACES[SPACE_BASE], .id = 0}, seed); }

// Load/Save

#define SPACE_BYTE_BUF_SIZE sizeof(Chunk) * WORLD_LOADED_CHUNKS + 100 + sizeof(BeingInstance) * 200

void space_load(SaveDescriptor save_desc, SpaceDescriptor space_desc, Space *space, DataContext ctx) {
  world_initialize(&space->world);
  LOAD_SPACE_DATA(save_desc, space_desc, "world", SPACE_BYTE_BUF_SIZE, byte_buf, {
    Data data_map = byte_buf_read_data(&byte_buf);
    DataMap *world_map = &data_map.var.data_map;
    world_load(&space->world, world_map, ctx);

    //data_free(&data_map);
  });
}

void space_save(SaveDescriptor save_desc, const Space *space, DataContext ctx) {
  SAVE_SPACE_DATA(save_desc, space->desc, "world", SPACE_BYTE_BUF_SIZE, byte_buf, {
    DataMap world_map = data_map_new(2000);
    world_save(&space->world, &world_map, ctx);

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
