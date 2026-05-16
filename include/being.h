#pragma once

#include "being/being_activities.h"
#include "being/being_memories.h"
#include "data.h"
#include "data/data_ex.h"
#include "item.h"
#include "shared.h"
#include <raylib.h>
#include <stdlib.h>
#include "registries/beings.h"

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
