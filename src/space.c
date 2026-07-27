#include "../include/space.h"
#include "../include/data/data_file.h"
#include "../include/data/load.h"
#include "../include/data/save.h"
#include "lilc/alloc.h"
#include "lilc/array.h"
#include "lilc/log.h"
#include "lilc/panic.h"
#include "lilc/str.h"

SpaceId space_id_by_ident(const char *name) {
  if (str_eq(name, "base")) {
    return SPACE_BASE;
  }
  if (str_eq(name, "dungeon")) {
    return SPACE_DUNGEON_TEST;
  }
  return (long)panic("Invalid space id: %s", name);
}

void space_init(Space *space, SpaceDescriptor desc, float seed) {
  space->desc = desc;
  printf("Creating space - Seed: %f, space id: %d\n", seed, desc.space_id);

  world_init(&space->world, desc.space_id, seed);
  if (!desc.external) {
    world_gen(&space->world);
  }

  space->beings = array_new_capacity(BeingInstance, 1024, &HEAP_ALLOCATOR);
}

void space_init_default(Space *space, float seed) { space_init(space, (SpaceDescriptor){.space_id = SPACE_BASE, .id = 0}, seed); }

// BEINGS

void space_add_being(Space *space, BeingInstance being) {
  size_t index = array_len(space->beings);
  array_add(space->beings, being);
  space->beings[index].being_instance_id = index;
}

void space_remove_being(Space *space, BeingInstance *being) {
  int index = 0;
  for (size_t i = 0; i < array_len(space->beings); i++) {
    if (space->beings[i].being_instance_id == being->being_instance_id) {
      index = i;
      break;
    }
  }
  array_remove(space->beings, index);
}

// Load/Save

static void space_save_beings(const Space *space, DataMap *data, DataContext ctx) {
  DataList list = data_list_new(array_len(space->beings), &HEAP_ALLOCATOR);
  for (size_t i = 0; i < array_len(space->beings); i++) {
    const BeingInstance *instance = &space->beings[i];
    DataMap being_data = data_map_new(10, &HEAP_ALLOCATOR);
    data_map_insert(&being_data, "id", data_int(instance->id));
    being_save(instance, &being_data, ctx);
    data_list_add(&list, data_map(being_data));
  }
  data_map_insert(data, "beings", data_list(list));
}

static void space_load_beings(Space *space, const DataMap *data, DataContext ctx) {
  DataList list = data_map_get_or_default(data, "beings", data_list(data_list_new(100, &HEAP_ALLOCATOR))).var.data_list;
  for (size_t i = 0; i < list.len; i++) {
    DataMap being_data = data_list_get(&list, i).var.data_map;
    BeingId id = data_map_get_or_default(&being_data, "id", data_int(BEING_NPC)).var.data_int;
    BeingInstance being_instance = being_new_default(id);
    being_load(&being_instance, &being_data, ctx);
    space_add_being(space, being_instance);
  }
}

static void data_map_free(DataMap *data) {
  Data d = data_map(*data);
  data_free(&d);
}

void space_load(SaveDescriptor save_desc, SpaceDescriptor space_desc, Space *space, DataContext ctx) {
  SpaceProperties space_props = ctx.registries->spaces[space_desc.space_id];

  {
    DataMap space_world_data;

    const char *path = str_fmt_temp("save/save%d/spaces/%s-%zu/world.bin", save_desc.id, space_props.ident, space_desc.id);
    data_file_read(path, NULL, &space_world_data, &HEAP_ALLOCATOR);

    world_load(&space->world, &space_world_data, ctx);
    world_initialize(&space->world);

    data_map_free(&space_world_data);
  }

  {
    DataMap space_beings_data;

    const char *path = str_fmt_temp("save/save%d/spaces/%s-%zu/beings.bin", save_desc.id, space_props.ident, space_desc.id);
    data_file_read(path, NULL, &space_beings_data, &HEAP_ALLOCATOR);

    space_load_beings(space, &space_beings_data, ctx);

    data_map_free(&space_beings_data);
  }
}

void space_save(SaveDescriptor save_desc, const Space *space, DataContext ctx) {
  SpaceProperties space_props = ctx.registries->spaces[space->desc.space_id];
  log_info("Saving space '%s' with id %d", space_props.ident, space->desc.space_id);

  {
    DataMap space_world_data = data_map_new(200, &HEAP_ALLOCATOR);

    world_save(&space->world, &space_world_data, ctx);
    log_info("Saved world");

    DataHeader header = {.version = 0};
    const char *path = str_fmt_temp("save/save%d/spaces/%s-%zu/world.bin", save_desc.id, space_props.ident, space->desc.id);
    data_file_write(path, &header, &space_world_data, &HEAP_ALLOCATOR);

    data_map_free(&space_world_data);
  }

  {
    DataMap space_beings_data = data_map_new(200, &HEAP_ALLOCATOR);

    space_save_beings(space, &space_beings_data, ctx);
    log_info("Saved beings");

    DataHeader header = {.version = 0};
    const char *path = str_fmt_temp("save/save%d/spaces/%s-%zu/beings.bin", save_desc.id, space_props.ident, space->desc.id);
    data_file_write(path, &header, &space_beings_data, &HEAP_ALLOCATOR);
  
    data_map_free(&space_beings_data);
  }
  log_info("Saved space");
}
