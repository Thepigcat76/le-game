#include "../include/camera.h"

Camera2D camera_new(int screen_width, int screen_height) {
  Camera2D cam;
  cam.target = (Vector2){screen_width / 2.0f, screen_height / 2.0f - 400};
  cam.offset = (Vector2){screen_width / 2.0f, screen_height / 2.0f};
  cam.rotation = 0.0f;
  cam.zoom = 1.0f;
  return cam;
}