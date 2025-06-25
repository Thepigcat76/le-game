#include "../include/being.h"
#include "../include/data.h"
#include "../include/data/data_ex.h"
#include <math.h>
#include <raylib.h>

static Vec2f being_width_height(BeingId id);

static BeingInstanceEx being_ex_default(BeingId id);

static bool being_has_brain(BeingId id);

BeingInstance being_new(BeingId id, BeingInstanceEx extra, int x, int y) {
  return (BeingInstance){
      .id = id,
      .extra = extra,
      .context = {.box = {.x = x, .y = y, .width = being_width_height(id).x, .height = being_width_height(id).y},
                  .removed = false,
                  .creation_time = GetTime()},
      .brain = {.brain_id = BEING_ACTIVITIES_ECS.next_id++},
      .has_brain = being_has_brain(id)};
}

BeingInstance being_new_default(BeingId id) { return being_new(id, being_ex_default(id), 0, 0); }

BeingInstance being_item_new(ItemInstance item, int x, int y) {
  return being_new(
      BEING_ITEM,
      (BeingInstanceEx){.type = BEING_INSTANCE_ITEM, .var = {.item_instance = {.item = item, .should_hover = true}}}, x,
      y);
}

BeingInstance being_npc_new(int x, int y) {
  BeingInstance being_instance = being_new(BEING_NPC,
                                           (BeingInstanceEx){.type = BEING_INSTANCE_NPC,
                                                             .var = {.npc_instance = {.variant = GetRandomValue(0, 1),
                                                                                      .direction = DIRECTION_DOWN,
                                                                                      .animation_frame = 0,
                                                                                      .frame_timer = 0,
                                                                                      .in_water = false,
                                                                                      .walking = true}}},
                                           x, y);
  TraceLog(LOG_DEBUG, "Created new npc, has brain: %s", being_instance.has_brain ? "true" : "false");
  return being_instance;
}

static Vec2f being_width_height(BeingId id) {
  switch (id) {
  case BEING_ITEM:
    return vec2f(16, 16);
  case BEING_NPC:
    return vec2f(16, 32);
  }
}

static BeingInstanceEx being_ex_default(BeingId id) {
  switch (id) {
  case BEING_NPC: {
    return (BeingInstanceEx){.type = BEING_INSTANCE_NPC,
                             .var = {.npc_instance = {.animation_frame = 0,
                                                      .frame_timer = 0,
                                                      .walking = false,
                                                      .in_water = false,
                                                      .direction = DIRECTION_DOWN}}};
  }
  case BEING_ITEM: {
    return (BeingInstanceEx){
        .type = BEING_INSTANCE_ITEM,
        .var = {.item_instance = {.item = (ItemInstance){.type = ITEMS[ITEM_STICK]}, .should_hover = false}}};
  }
  }
}

static bool being_has_brain(BeingId id) {
  switch (id) {
  case BEING_ITEM:
    return false;
  case BEING_NPC:
    return true;
  }
}

void being_tick(BeingInstance *being) {
  if (being->has_brain) {
    being_brain_tick(being, &being->brain);
  }
}

#define BEING_SPEED 0.5

static void being_go_to(BeingInstance *being, Vec2f pos) {
  Rectangle *being_box = &being->context.box;
  float sig_x = signum(pos.x - being_box->x);
  float sig_y = signum(pos.y - being_box->y);

  being_box->x += BEING_SPEED * sig_x;
  being_box->y += BEING_SPEED * sig_y;

  if (being->id == BEING_NPC) {
    being->extra.var.npc_instance.direction = direction_from_delta(sig_x, sig_y);
  }
}

static void being_activity_go_to_pos(BeingInstance *being, BeingActivityGoToPosition *activity) {
  if (!activity->present)
    return;

  being_go_to(being, activity->val.target_position);
}

static void being_activity_wa_find_next_pos(BeingInstance *being, BeingActivityWalkAround *activity) {
  Vec2i next_pos_range_min = vec2i(-5, -5);
  Vec2i next_pos_range_max = vec2i(5, 5);

  int x = GetRandomValue(next_pos_range_min.x, next_pos_range_max.x);
  int y = GetRandomValue(next_pos_range_min.y, next_pos_range_max.y);

  activity->val.prev_target_pos = activity->val.cur_target_pos;
  activity->val.cur_target_pos = vec2f(being->context.box.x + x * TILE_SIZE, being->context.box.y + y * TILE_SIZE);
}

#define BEING_GET_ACTIVITY(being_ptr, activity_name)                                                                   \
  BEING_ACTIVITIES_ECS.activities_##activity_name[being->brain.brain_id]

static void being_activity_walk_around(BeingInstance *being, BeingActivityWalkAround *activity) {
  if (!activity->present)
    return;
  BeingActivityIdle idle_activity = BEING_GET_ACTIVITY(being, idle);
  if (!idle_activity.present || idle_activity.val.ended) {
    being_go_to(being, activity->val.cur_target_pos);
    if (fabs(being->context.box.x - activity->val.cur_target_pos.x) < 1 &&
        fabs(being->context.box.y - activity->val.cur_target_pos.y) < 1) {
      being_activities_add_idle(being, 200);
      being_activity_wa_find_next_pos(being, activity);
    }
  }
}

static void being_activity_idle(BeingInstance *being, BeingActivityIdle *activity) {
  if (!activity->present)
    return;

  if (activity->val.timer < activity->val.idle_time) {
    activity->val.timer++;
  } else {
    activity->val.ended = true;
  }
}

#define ACTIVITY_TICK(activity_name)                                                                                   \
  being_activity_##activity_name(being, &BEING_ACTIVITIES_ECS.activities_##activity_name[brain->brain_id])

void being_brain_tick(BeingInstance *being, BeingBrain *brain) {
  ACTIVITY_TICK(walk_around);
  ACTIVITY_TICK(idle);
  ACTIVITY_TICK(go_to_pos);
}

static Texture2D being_npc_get_texture(BeingInstanceExNpc *being_ex) {
  if (being_ex->variant == OLD_MAN) {
    return NPC_TEXTURE_OLD_MAN;
  }

  Texture2D *textures;

  if (being_ex->walking) {
    textures = NPC_ANIMATED_TEXTURES;
  } else {
    textures = NPC_TEXTURES;
  }

  switch (being_ex->direction) {
  case DIRECTION_DOWN:
    return textures[0];
  case DIRECTION_UP:
    return textures[1];
  case DIRECTION_LEFT:
    return textures[2];
  case DIRECTION_RIGHT:
    return textures[3];
  }
}

static void npc_update_animation(BeingInstanceExNpc *npc, float deltaTime) {
  npc->frame_timer += deltaTime * 1000.0f; // to ms
  float delay = 125;
  int frames = npc->variant == OLD_MAN ? 2 : 5;
  if (npc->frame_timer >= delay) {
    npc->frame_timer = 0.0f;
    npc->animation_frame = (npc->animation_frame + 1) % frames;
  }
}

void being_render(BeingInstance *being) {
  switch (being->id) {
  case BEING_ITEM: {
    float hover_offset = 0;
    if (being->extra.var.item_instance.should_hover) {
      float amplitude = 2.0f;
      float speed = 5.0f;
      float t = (GetTime() - being->context.creation_time) * speed;
      hover_offset = sinf(t) * fabs(sinf(t)); // sin^2 with sign
      hover_offset *= amplitude;
    }
    DrawTexture(being->extra.var.item_instance.item.type.texture, being->context.box.x,
                being->context.box.y + hover_offset, WHITE);
    break;
  }
  case BEING_NPC: {
    double scale = 1;
    BeingInstanceExNpc *ex = &being->extra.var.npc_instance;
    Texture2D being_texture = being_npc_get_texture(ex);
    DrawTextureRecEx(being_texture, rectf(0, ex->walking ? 32 * ex->animation_frame : 32, 16, ex->in_water ? 24 : 32),
                     vec2f(being->context.box.x + 8 * scale, being->context.box.y + 16 * scale), 0, scale, WHITE);

    if (ex->walking || being->extra.var.npc_instance.variant == OLD_MAN) {
      npc_update_animation(ex, GetFrameTime());
    }
    break;
  }
  }
}

void being_brain_reset(BeingInstance *being) {}

void being_add_memory(BeingInstance *being, BeingMemory memory) {}

static void being_load_ex(BeingInstanceEx *extra, DataMap *data) {
  switch (extra->type) {
  case BEING_INSTANCE_ITEM: {
    extra->var.item_instance.item =
        (ItemInstance){.type = ITEMS[data_map_get_or_default(data, "item_id", data_int(ITEM_EMPTY)).var.data_int]};
    extra->var.item_instance.should_hover =
        data_map_get_or_default(data, "should_hover", data_byte(false)).var.data_byte;
    break;
  }
  case BEING_INSTANCE_NPC: {
    extra->var.npc_instance.variant = data_map_get_or_default(data, "npc_var", data_int(BROTHER)).var.data_int;
    extra->var.npc_instance.direction =
        data_map_get_or_default(data, "direction", data_int(DIRECTION_DOWN)).var.data_int;
    extra->var.npc_instance.walking = data_map_get_or_default(data, "walking", data_byte(false)).var.data_byte;
    break;
  }
  case BEING_INSTANCE_DEFAULT: {
    break;
  }
  }
}

void being_load(BeingInstance *being, const DataMap *data) {
  DataMap extra_data_map = data_map_get_or_default(data, "extra", data_map(data_map_new(0))).var.data_map;
  being_load_ex(&being->extra, &extra_data_map);
  // being->context.creation_time = data_map_get_or_default(data, "creation_time", data_int(0)).var.data_int;
  being->context.box =
      data_map_get_rectf_static_dimensions(data, "box", being->context.box.width, being->context.box.height);
}

static void being_save_ex(const BeingInstanceEx *extra, DataMap *data) {
  switch (extra->type) {
  case BEING_INSTANCE_ITEM: {
    data_map_insert(data, "item_id", data_int(extra->var.item_instance.item.type.id));
    data_map_insert(data, "should_hover", data_byte(extra->var.item_instance.should_hover));
    break;
  }
  case BEING_INSTANCE_NPC: {
    data_map_insert(data, "npc_var", data_int(extra->var.npc_instance.variant));
    data_map_insert(data, "direction", data_int(extra->var.npc_instance.direction));
    data_map_insert(data, "walking", data_byte(extra->var.npc_instance.walking));
    break;
  }
  case BEING_INSTANCE_DEFAULT: {
    break;
  }
  }
}

void being_save(const BeingInstance *being, DataMap *data) {
  // Being id is already initialized
  // Don't save being_instance_id, cuz we can assign that after loading
  // extra
  DataMap extra_data_map = data_map_new(20);
  being_save_ex(&being->extra, &extra_data_map);
  data_map_insert(data, "extra", data_map(extra_data_map));
  // context
  // TODO: Reenable creation time serialization after we serialize game time
  // data_map_insert(data, "creation_time", data_int(being->context.creation_time));
  data_map_insert_rectf_static_dimensions(data, "box", being->context.box);
  // TODO: Save brain
}
