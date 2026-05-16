#pragma once

#include "lilc/alloc.h"
#include "lilc/bump.h"
#include "registries/items.h"
#include "registries/tiles.h"
#include "registries/beings.h"
#include "registries/worlds.h"

typedef struct {
  bool initialized;

  ItemProperties *items;
  TileProperties *tiles;
  BeingProperties *beings;
  WorldProperties *worlds;

  Bump registry_bump;
  Allocator registry_bump_allocator;
} RegistryManager;

void registries_load(RegistryManager *manager);

void registries_unload(RegistryManager *manager);
