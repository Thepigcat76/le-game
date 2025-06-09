#pragma once

#include <raylib.h>

Camera2D camera_new(int screen_width, int screen_height);

void camera_focus(Camera2D *cam);
