#include "../../include/assets/sounds.h"
#include "../../include/assets.h"
#include "lilc/array.h"
#include "lilc/log.h"
#include <raylib.h>

AssetId SOUND_IDS[_amount_sound_handles];

static AssetId sound_asset_id(AssetManager *m, char *filename) {
  cw_Sound *sound;
  array_foreach(m->sounds, sound) {
    if (str_eq(sound->path, TextFormat(ASSETS_DIR SOUNDS_DIR "/%s.wav", filename))) {
      return sound->id;
    }
  }
  return 0;
}

void sound_handles_assign_id(AssetManager *m) {
  SOUND_IDS[SOUND_PLACE] = sound_asset_id(m, "place_sound");
}

i32 cw_sound_load(cw_Sound *sound, AssetManager *manager, FileEntry file_entry) {
  sound->sound = LoadSound(file_entry.full_path);
  if (sound->sound.frameCount == 0) {
    log_error("Failed to load sound %s", file_entry.full_path);
    return 0;
  }

  sound->path = str_cpy(file_entry.full_path, &manager->asset_bump_allocator);
  
  return 1;
}

void cw_sound_unload(cw_Sound *sound) {
  UnloadSound(sound->sound);
}
