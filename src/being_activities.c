#include "../include/being.h"

BeingActivitiesEcs BEING_ACTIVITIES_ECS = {};

void being_activities_add_idle(BeingInstance *being, int idle_time) {
  BEING_ACTIVITIES_ECS.activities_idle[being->brain.brain_id] = COMPONENT_CREATE(BeingActivityIdle, {.idle_time = idle_time});
}

void being_activities_add_walk_around(BeingInstance *being, Vec2f target_pos) {
  BEING_ACTIVITIES_ECS.activities_walk_around[being->brain.brain_id] = COMPONENT_CREATE(BeingActivityWalkAround, {.cur_target_pos = target_pos});
}

void being_activities_add_go_to_pos(BeingInstance *being, Vec2f target_pos) {
  BEING_ACTIVITIES_ECS.activities_go_to_pos[being->brain.brain_id] = COMPONENT_CREATE(BeingActivityGoToPosition, {.target_position = target_pos});
}
