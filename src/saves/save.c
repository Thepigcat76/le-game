#include "../../include/save.h"
#include "lilc/array.h"
#include "lilc/log.h"
#include <raylib.h>

// TODO: create bumps for both of these possibly
Save save_new(SaveDescriptor desc) {
  return (Save){.descriptor = desc,
                //.players = array_new(Player, &HEAP_ALLOCATOR),
                .spaces = array_new_capacity(SpaceDescriptor, 16, &HEAP_ALLOCATOR),
                .loaded_spaces = array_new_capacity(Space, 16, &HEAP_ALLOCATOR),
                .space_id_lookup = {.entries = array_new_capacity(SpaceIdLookupEntry, 16, &HEAP_ALLOCATOR)}};
}

cJSON *save_config_to_json(const SaveConfig *config) {
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "name", config->save_name);
  {
    cJSON *world_json = cJSON_AddObjectToObject(json, "world");
    cJSON_AddNumberToObject(world_json, "seed", config->seed);
  }
  return json;
}

static size_t space_disk_id_from_str(const char *path) {
  const char *id_lit = strrchr(path, '-');
  size_t id = atoi(id_lit + 1);
  return id;
}

static size_t space_disk_id_from_id(const SpaceIdLookup *lookup, SpaceId space_id) { return 0; }

void save_scan_spaces(Save *save, DataContext ctx) {
  char path[256];
  if (save->descriptor.is_server_save) {
    strcpy(path, "server-save/spaces");
  } else {
    strcpy(path, TextFormat("save/save%d/spaces", save->descriptor.id));
  }
  DIR_ITER(path, entry, {
    printf("Found space: %s\n", entry->d_name);
    char path_buf[strlen(entry->d_name) + 1];
    strcpy(path_buf, entry->d_name);
    char *path_buf_end_of_id_lit = strrchr(path_buf, '-');
    *path_buf_end_of_id_lit = '\0';
    SpaceId space_id = space_id_by_ident(path_buf);
    SpaceDescriptor desc = {.space_id = space_id, .id = space_disk_id_from_str(entry->d_name), .external = true};
    array_add(save->spaces, desc);
    SpaceProperties space_props = ctx.registries->spaces[space_id];
    TraceLog(LOG_INFO, "Found save: Type: %s with index: %zu", space_props.name, desc.id);
    ssize_t space_id_index = -1;
    SpaceIdLookupEntry *entry;
    array_foreach(save->space_id_lookup.entries, entry) {
      if (entry->space_id == space_id) {
        space_id_index = _arr_foreach_idx;
        break;
      }
    };

    if (space_id_index == -1) {
      size_t *disk_ids = array_new_capacity(size_t, 8, &HEAP_ALLOCATOR);
      array_add(save->space_id_lookup.entries, (SpaceIdLookupEntry){.space_id = space_id, .disk_ids = disk_ids});
      array_add(disk_ids, desc.id);
    }
  });

  SpaceDescriptor *space_desc;
  array_foreach(save->spaces, space_desc) {
    log_debug("Space: %d - disk id: %zu\n", space_desc->space_id, space_desc->id);
  }
  
}
