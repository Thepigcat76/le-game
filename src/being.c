#include "../include/being.h"
#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

BeingInstance being_new(BeingId id, BeingInstanceEx extra, int x, int y, int width, int height) {
  return (BeingInstance){.id = id,
                         .extra = extra,
                         .context = {.box = {.x = x, .y = y, .width = width, .height = height},
                                     .removed = false,
                                     .creation_time = GetTime()},
                         .world = NULL,
                         .brain = {0},
                         .has_brain = false};
}

BeingInstance being_item_new(ItemInstance item, int x, int y) {
  return being_new(BEING_ITEM, (BeingInstanceEx){.type = BEING_INSTANCE_ITEM, .var = {.item_instance = {.item = item}}},
                   x, y, 16, 16);
}

BeingInstance being_npc_new(int x, int y) {
  BeingInstance being_instance = being_new(BEING_NPC,
                                           (BeingInstanceEx){.type = BEING_INSTANCE_NPC,
                                                             .var = {.npc_instance = {.direction = DIRECTION_DOWN,
                                                                                      .animation_frame = 0,
                                                                                      .frame_timer = 0,
                                                                                      .in_water = false,
                                                                                      .walking = true}}},
                                           x, y, 16, 32);
  return being_instance;
}

void being_tick(BeingInstance *being) { being_brain_tick(being, &being->brain); }

void being_brain_tick(BeingInstance *being, BeingBrain *brain) {
  for (size_t i = 0; i < brain->activities_amount; i++) {
    being_activity_tick(being, &brain->activities[i]);
  }
}

void being_activity_tick(BeingInstance *being, BeingActivity *activity) {
  switch (activity->type) {
  case BEING_ACTIVITY_WALK_AROUND: {
    break;
  }
  case BEING_ACTIVITY_GO_TO_POSITION: {
    break;
  }
  }
}

static Texture2D being_npc_get_texture(BeingInstanceExNpc *being_ex) {
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

static void npc_update_animation(BeingInstanceExNpc *player, float deltaTime) {
  player->frame_timer += deltaTime * 1000.0f; // to ms
  float delay = 125;
  if (player->frame_timer >= delay) {
    player->frame_timer = 0.0f;
    player->animation_frame = (player->animation_frame + 1) % 5;
  }
}

void being_render(BeingInstance *being) {
  switch (being->id) {
  case BEING_ITEM: {
    float amplitude = 2.0f;
    float speed = 5.0f;
    float t = (GetTime() - being->context.creation_time) * speed;
    float hover_offset = sinf(t) * fabs(sinf(t)); // sin^2 with sign
    hover_offset *= amplitude;
    DrawTexture(being->extra.var.item_instance.item.type.texture, being->context.box.x,
                being->context.box.y + hover_offset, WHITE);
    break;
  }
  case BEING_NPC: {
    double scale = 1;
    BeingInstanceExNpc *ex = &being->extra.var.npc_instance;
    Texture2D being_texture = being_npc_get_texture(ex);
    DrawTexturePro(being_texture,
                   (Rectangle){0, ex->walking ? 32 * ex->animation_frame : 32, 16, ex->in_water ? 24 : 32},
                   (Rectangle){.x = being->context.box.x + 8 * scale,
                               .y = being->context.box.y + 16 * scale,
                               .width = 16 * scale,
                               .height = (ex->in_water ? 24 : 32) * scale},
                   (Vector2){.x = 8 * scale, .y = 16 * scale}, 0, WHITE);

    if (ex->walking) {
      npc_update_animation(ex, GetFrameTime());
    }
    break;
  }
  }
}
