#pragma once

#include "item.h"
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

typedef struct being_instance {
  BeingId id;
  BeingInstanceEx extra;
  BeingContext context;
  struct world *world;
} BeingInstance;

BeingInstance being_new(BeingId id, BeingInstanceEx extra, int x, int y, int width, int height);

BeingInstance being_item_new(ItemInstance item, int x, int y);

void being_tick(BeingInstance *being);

void being_render(BeingInstance *being);
