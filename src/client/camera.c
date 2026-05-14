#include "../../include/camera.h"
#include "../../include/shared.h"
#include "lilc/numbers.h"
#include <raylib.h>

void camera_init(Camera2D *cam, i32 screen_width, i32 screen_height) {
  cam->target = (Vector2){screen_width / 2.0f - 400, screen_height / 2.0f - 400};
  cam->offset = (Vector2){screen_width / 2.0f, screen_height / 2.0f};
  cam->rotation = 0.0f;
  cam->zoom = 1.0f;
}

void camera_focus(Camera2D *cam) {
  cam->offset = vec2f(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f);
}
