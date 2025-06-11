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
  int idle_time;
  int timer;
  bool ended;
} BeingActivityIdle;

typedef struct {
  enum {
    BEING_ACTIVITY_WALK_AROUND,
    BEING_ACTIVITY_GO_TO_POSITION,
    BEING_ACTIVITY_IDLE,
  } type;
  union {
    BeingActivityWalkAround activity_walk_around;
    BeingActivityGoToPosition activity_go_to_pos;
    BeingActivityIdle activity_idle;
  } var;
} BeingActivity;
