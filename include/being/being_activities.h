#pragma once

#include "../shared.h"
#include <stdint.h>

#define COMPONENT_MAX_AMOUNT 256

#define _COMPONENT_FIELD(type, name) type name[COMPONENT_MAX_AMOUNT]

#define COMPONENT_CREATE(type, ...) (type){.val = __VA_ARGS__, .present = true}

#define COMPONENT(name, fields)                                                                                        \
  typedef struct {                                                                                                     \
    struct fields val;                                                                                                 \
    bool present;                                                                                                      \
  } name

COMPONENT(BeingActivityIdle, {
  int idle_time;
  int timer;
  bool ended;
});

COMPONENT(BeingActivityWalkAround, {
    Vec2f prev_target_pos;
    Vec2f cur_target_pos;
});

COMPONENT(BeingActivityGoToPosition, {
    Vec2f target_position;
});

typedef struct {
  uint32_t next_id;
  _COMPONENT_FIELD(BeingActivityIdle, activities_idle);
  _COMPONENT_FIELD(BeingActivityWalkAround, activities_walk_around);
  _COMPONENT_FIELD(BeingActivityGoToPosition, activities_go_to_pos);
} BeingActivitiesEcs;

extern BeingActivitiesEcs BEING_ACTIVITIES_ECS;
