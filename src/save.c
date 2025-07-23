#include "../include/save.h"
#include "../include/array.h"
#include <raylib.h>

// TODO: create bumps for both of these possibly
Save save_new(SaveDescriptor desc) {
  return (Save){.descriptor = desc,
                .player = player_new(),
                .spaces = array_new_capacity(SpaceDescriptor, 16, &HEAP_ALLOCATOR),
                .loaded_spaces = array_new_capacity(Space, 16, &HEAP_ALLOCATOR),
                .space_id_lookup = {.entries = array_new_capacity(SpaceIdLookupEntry, 16, &HEAP_ALLOCATOR)}};
}

static const SpaceType *space_type_from_id(SpaceId space_id) {
  size_t len = array_len(SPACES);
  for (size_t i = 0; i < len; i++) {
    if (SPACES[i].space_id == space_id) {
      return &SPACES[i];
    }
  }
  return NULL;
}

static size_t space_disk_id_from_str(const char *path) {
  const char *id_lit = strrchr(path, '-');
  size_t id = atoi(id_lit + 1);
  return id;
}

static size_t space_disk_id_from_id(const SpaceIdLookup *lookup, SpaceId space_id) { return 0; }

void save_load_spaces(Save *save) {
  DIR_ITER(TextFormat("save/save%d/spaces", save->descriptor.id), entry, {
    char path_buf[strlen(entry->d_name) + 1];
    strcpy(path_buf, entry->d_name);
    char *path_buf_end_of_id_lit = strrchr(path_buf, '-');
    *path_buf_end_of_id_lit = '\0';
    SpaceId space_id = space_id_by_name(path_buf);
    SpaceDescriptor desc = {.type = space_type_from_id(space_id), .id = space_disk_id_from_str(entry->d_name)};
    array_add(save->spaces, desc);
    TraceLog(LOG_INFO, "Found save: Type: %s with index: %zu", space_id_to_name(space_id), desc.id);
    ssize_t space_id_index = -1;
    array_foreach(save->space_id_lookup.entries, SpaceIdLookupEntry, entry, {
      if (entry.space_id == space_id) {
        space_id_index = _arr_foreach_index;
        break;
      }
    });

    if (space_id_index == -1) {
      size_t *disk_ids = array_new_capacity(size_t, 8, &HEAP_ALLOCATOR);
      array_add(save->space_id_lookup.entries, (SpaceIdLookupEntry){.space_id = space_id, .disk_ids = disk_ids});
      array_add(disk_ids, desc.id);
    }
  });

  printf("-- Space id lookup --\n");
  array_foreach(save->space_id_lookup.entries, SpaceIdLookupEntry, entry, {
    printf("  Space: %s\n", space_id_to_name(entry.space_id));
    array_foreach(save->space_id_lookup.entries->disk_ids, size_t, entry, {
      printf("    Disk id: %zu\n", entry);
    });
  });

  PANIC_FMT("look at da lookup :3");
}
