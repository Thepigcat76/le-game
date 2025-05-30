#include "../include/being.h"
#include "../include/data.h"
#include "../include/data_ex.h"
#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

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
      .brain = {0},
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
  // TraceLog(LOG_INFO, "tick being");
  if (being->has_brain) {
    being_brain_tick(being, &being->brain);
  }
}

void being_brain_tick(BeingInstance *being, BeingBrain *brain) {
  for (size_t i = 0; i < brain->activities_amount; i++) {
    TraceLog(LOG_INFO, "tick brain");
    being_activity_tick(being, &brain->activities[i]);
  }
}

static void being_activity_walk_around(BeingInstance *being_instance, BeingActivityWalkAround *activity) {
  Rectangle *being_box = &being_instance->context.box;
  float sig_x = signum(activity->target_position.x - being_box->x);
  float sig_y = signum(activity->target_position.y - being_box->y);

  being_box->x += 0.75 * sig_x;
  being_box->y += 0.75 * sig_y;
  TraceLog(LOG_DEBUG, "new box pos: %f", being_box->x, being_box->y);

  if (being_instance->id == BEING_NPC) {
    being_instance->extra.var.npc_instance.direction = direction_from_delta(sig_x, sig_y);
  }
}

void being_activity_tick(BeingInstance *being, BeingActivity *activity) {
  switch (activity->type) {
  case BEING_ACTIVITY_WALK_AROUND: {
    TraceLog(LOG_INFO, "Walk around");
    being_activity_walk_around(being, &activity->var.activity_walk_around);
    break;
  }
  case BEING_ACTIVITY_GO_TO_POSITION: {
    break;
  }
  }
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

void being_brain_reset(BeingInstance *being) {
  BeingBrain *brain = &being->brain;
  brain->activities_amount = 0;
  brain->memories_amount = 0;
}

void being_add_activity(BeingInstance *being, BeingActivity activity) {
  if (being->has_brain) {
    BeingBrain *brain = &being->brain;
    brain->activities[brain->activities_amount++] = activity;
  }
}

void being_add_memory(BeingInstance *being, BeingMemory memory) {}

static void being_load_ex(BeingInstanceEx *extra, DataMap *data) {
  switch (extra->type) {
  case BEING_INSTANCE_ITEM: {
    extra->var.item_instance.item =
        (ItemInstance){.type = ITEMS[data_map_get_or_default(data, "item_id", data_int(ITEM_EMPTY)).var.data_int]};
    extra->var.item_instance.should_hover = data_map_get_or_default(data, "should_hover", data_byte(false)).var.data_byte;
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
  //being->context.creation_time = data_map_get_or_default(data, "creation_time", data_int(0)).var.data_int;
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
  //data_map_insert(data, "creation_time", data_int(being->context.creation_time));
  data_map_insert_rectf_static_dimensions(data, "box", being->context.box);
  // TODO: Save brain
}
