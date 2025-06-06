#pragma once

#include "game_feature_content.h"
#include <stdlib.h>

typedef enum {
  GAME_FEATURE_VILLAGE,
  GAME_FEATURE_HOUSE,
  GAME_FEATURE_NPC,
  GAME_FEATURE_FOREST,
  GAME_FEATURE_TREE,
} GameFeatureId;

typedef struct {
  GameFeatureId id;
  // Other properties
} GameFeatureType;

typedef struct {
  GameFeatureId type;
  union {
    GameFeatureContentVillage village;
    GameFeatureContentForest forest;
    GameFeatureContentTree tree;
    GameFeatureContentNpc npc;
  } var;
} GameFeatureContent;

typedef struct _game_feature {
  GameFeatureType type;
  GameFeatureContent content;
  struct _game_feature *children;
} GameFeature;

typedef struct {
  GameFeature *game_features;
  size_t game_features_amount;
  size_t game_features_capacity;
} GameFeatureStore;
