#pragma once

#include "item.h"
#include "world.h"
#include <raylib.h>

typedef enum {
  BEING_ITEM,
} BeingId;

typedef struct {
  Rectangle box;
  bool removed;
  double creation_time;
} BeingContext;

typedef struct {
  enum {
    BEING_INSTANCE_DEFAULT,
    BEING_INSTANCE_ITEM,
  } type;
  union {
    struct {
    } default_instance;
    struct {
      ItemInstance item;
    } item_instance;
  } var;
} BeingInstanceEx;

typedef struct {
  BeingId id;
  BeingInstanceEx extra;
  BeingContext context;
  World *world;
} BeingInstance;

BeingInstance being_new(BeingId id, BeingInstanceEx extra, int x, int y, int width, int height);

void being_tick(BeingInstance *being);

void being_render(BeingInstance *being);
