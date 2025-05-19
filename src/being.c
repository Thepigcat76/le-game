#include "../include/being.h"
#include <math.h>
#include <raylib.h>

BeingInstance being_new(BeingId id, BeingInstanceEx extra, int x, int y,
                        int width, int height) {
  return (BeingInstance){
      .id = id,
      .extra = extra,
      .context = {.box = {.x = x, .y = y, .width = width, .height = height},
                  .removed = false,
                  .creation_time = GetTime()},
      .world = NULL};
}

void being_tick(BeingInstance *being) {}

void being_render(BeingInstance *being) {
  switch (being->id) {
  case BEING_ITEM: {
    float amplitude = 2.0f;
    float speed = 5.0f;
    float t = (GetTime() - being->context.creation_time) * speed;
    float hover_offset = sinf(t) * fabs(sinf(t)); // sin^2 with sign
    hover_offset *= amplitude;
    DrawTexture(
        being->extra.var.item_instance.item.type.texture, being->context.box.x,
        being->context.box.y + hover_offset, WHITE);
    break;
  }
  }
}
