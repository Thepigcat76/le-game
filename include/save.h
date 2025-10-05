#pragma once

#include "game_feature.h"
#include "player.h"
#include "space.h"
#include "save_desc.h"
#include "space_desc.h"

typedef struct {
  SaveDescriptor descriptor;
  Space *loaded_spaces;
  SpaceDescriptor *spaces;
  Player *players;
  GameFeatureStore feature_store;
  SpaceIdLookup space_id_lookup;
} Save;

// Create new save and return it
Save save_new(SaveDescriptor desc);

void save_load_spaces(Save *save);
