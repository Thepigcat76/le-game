#pragma once

#include <raylib.h>

typedef struct {
  enum {
    TEXTURE_STATIC,
    TEXTURE_ANIMATED,
  } type;
  union {
    Texture2D texture_static;
    struct {
      Texture2D texture;
      int frames;
      int frame_time;
    } texture_animated;
  } var;
  int width;
  int height;
} AdvTexture;

AdvTexture adv_texture_load(const char *path);

void adv_texture_unload(AdvTexture texture);

Texture2D adv_texture_to_texture(AdvTexture texture);
