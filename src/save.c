#include "../include/save.h"
#include "../include/array.h"
#include <raylib.h>

// TODO: create bumps for both of these possibly
Save save_new(SaveDescriptor desc) {
  return (Save){
      .descriptor = desc,
      .player = player_new(),
      .spaces = array_new_capacity(SpaceDescriptor, 8, &HEAP_ALLOCATOR),
      .loaded_spaces = array_new_capacity(Space, 8, &HEAP_ALLOCATOR),
  };
}

void save_load_spaces(Save *save) {
  DIR_ITER(TextFormat("save/save%d/spaces", save->descriptor.id), entry, {
    SpaceDescriptor desc = {.space_id = space_id_by_name(entry->d_name), .lazy_loaded = false};
    array_add(save->spaces, desc);
  });
}
