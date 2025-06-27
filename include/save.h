#pragma once

#include "game_feature.h"
#include "player.h"
#include "world.h"

typedef struct {
  char *save_name;
  float seed;
} SaveConfig;

typedef struct {
    int id;
    SaveConfig config;
} SaveDescriptor;

typedef struct {
  SaveDescriptor descriptor;
  World world;
  Player player;
  GameFeatureStore feature_store;
} Save;
