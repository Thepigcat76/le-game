#include "../include/camera.h"
#include <raylib.h>

Camera2D camera_new(int screen_width, int screen_height) {
  Camera2D cam;
  cam.target = (Vector2){screen_width / 2.0f - 400, screen_height / 2.0f - 400};
  cam.offset = (Vector2){screen_width / 2.0f, screen_height / 2.0f};
  cam.rotation = 0.0f;
  cam.zoom = 1.0f;
  return cam;
}

void camera_focus(Camera2D *cam) {
  cam->offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}
