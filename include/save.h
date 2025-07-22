#pragma once

#include "game_feature.h"
#include "player.h"
#include "space.h"
#include "save_desc.h"

typedef struct {
  SaveDescriptor descriptor;
  Space *cur_space;
  Space *loaded_spaces;
  SpaceDescriptor *spaces;
  Player player;
  GameFeatureStore feature_store;
} Save;

Save save_new(SaveDescriptor desc);

// Load space descriptors in save
void save_load_spaces(Save *save);
