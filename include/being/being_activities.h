#pragma once

#include "../shared.h"

typedef struct {
    Vec2f prev_target_pos;
    Vec2f cur_target_pos;
} BeingActivityWalkAround;

typedef struct {
    Vec2f target_position;
} BeingActivityGoToPosition;

typedef struct {
  enum {
    BEING_ACTIVITY_WALK_AROUND,
    BEING_ACTIVITY_GO_TO_POSITION,
  } type;
  union {
    BeingActivityWalkAround activity_walk_around;
    BeingActivityGoToPosition activity_go_to_position;
  } var;
} BeingActivity;
