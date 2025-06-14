#pragma once

#include "being/being_activities.h"
#include "being/being_memories.h"
#include "data.h"
#include "item.h"
#include "shared.h"
#include <raylib.h>
#include <stdlib.h>

typedef enum {
  BEING_ITEM,
  BEING_NPC,
} BeingId;

typedef struct {
  Rectangle box;
  bool removed;
  double creation_time;
} BeingContext;

// TODO: Proper naming
typedef enum {
  BROTHER,
  OLD_MAN,
} NpcVariant;

typedef struct {
  NpcVariant variant;
  Direction direction;
  int animation_frame;
  int frame_timer;
  bool walking;
  bool in_water;
} BeingInstanceExNpc;

typedef struct {
  enum {
    BEING_INSTANCE_DEFAULT,
    BEING_INSTANCE_ITEM,
    BEING_INSTANCE_NPC,
  } type;
  union {
    struct {
    } default_instance;
    struct {
      ItemInstance item;
      bool should_hover;
    } item_instance;
    BeingInstanceExNpc npc_instance;
  } var;
} BeingInstanceEx;

typedef struct {
  uint32_t brain_id;
} BeingBrain;

typedef struct _being_instance {
  BeingId id;
  int being_instance_id;
  BeingInstanceEx extra;
  BeingContext context;
  BeingBrain brain;
  bool has_brain;
} BeingInstance;

// BEING CREATION

BeingInstance being_new(BeingId id, BeingInstanceEx extra, int x, int y);

BeingInstance being_new_default(BeingId id);

BeingInstance being_item_new(ItemInstance item, int x, int y);

BeingInstance being_npc_new(int x, int y);

// BEING RENDERING

void being_render(BeingInstance *being);

// BEING TICKING

void being_tick(BeingInstance *being);

void being_brain_tick(BeingInstance *being, BeingBrain *brain);

// BEING BRAIN STUFFS

void being_brain_reset(BeingInstance *being);

void being_activities_add_idle(BeingInstance *being, int idle_time);

void being_activities_add_walk_around(BeingInstance *being, Vec2f target_pos);

void being_activities_add_go_to_pos(BeingInstance *being, Vec2f target_pos);

void being_add_memory(BeingInstance *being, BeingMemory memory);

// LOADING/SAVING
void being_load(BeingInstance *being, const DataMap *data);

void being_save(const BeingInstance *being, DataMap *data);
