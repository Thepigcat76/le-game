#pragma once

#include "lilc/alloc.h"
#include "lilc/bump.h"
#include "registries/items.h"
#include "registries/tiles.h"
#include "registries/beings.h"
#include "registries/spaces.h"
#include "registries/menus.h"

typedef struct {
  bool initialized;

  ItemProperties *items;
  TileProperties *tiles;
  BeingProperties *beings;
  SpaceProperties *spaces;
  MenuProperties *menus;

  Bump registry_bump;
  Allocator registry_bump_allocator;
} RegistryManager;

void registries_load(RegistryManager *manager);

void registries_unload(RegistryManager *manager);
