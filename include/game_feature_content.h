#pragma once

#include "shared.h"

typedef struct {
} ForestType;

typedef struct {
    int village_size;
    int residents;
    int development;
} GameFeatureContentVillage;

typedef struct {
    TilePos tree_pos;
} GameFeatureContentTree;

typedef struct {
    
} GameFeatureContentNpc;

typedef struct {
    ForestType type;
    int size;
    int rarity;
    int plant_varity;
} GameFeatureContentForest;
