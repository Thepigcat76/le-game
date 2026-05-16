#pragma once

#include "items.h"

typedef enum {
  BEING_ITEM,
  BEING_NPC,

  _amount_being_ids,
} BeingId;

typedef struct {
  BeingId id;
  bool has_brain;
} BeingProperties;

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

void being_init(BeingInstance *inst, BeingId id);
