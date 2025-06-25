#pragma once

#include <stdlib.h>
#include "raylib.h"

#define ANIMATED_TEXTURES_AMOUNT 4

typedef struct {
  enum {
    TEXTURE_STATIC,
    TEXTURE_ANIMATED,
  } type;
  union {
    Texture2D texture_static;
    struct {
      int animated_texture_id;
      Texture2D texture;
      int frames;
      int frame_time;
    } texture_animated;
  } var;
  int width;
  int height;
  const char *path;
} AdvTexture;

typedef struct {
  AdvTexture texture;
  int animated_texture_id;
  int cur_frame;
  float frame_timer;
} AnimatedTexture;

extern AnimatedTexture ANIMATED_TEXTURES[ANIMATED_TEXTURES_AMOUNT];
extern size_t ANIMATED_TEXTURES_LEN;

AdvTexture adv_texture_load(const char *path);

void adv_texture_unload(AdvTexture texture);

int adv_texture_cur_frame(const AdvTexture *texture);

int adv_texture_frame_height(const AdvTexture *texture);

Texture2D adv_texture_to_texture(const AdvTexture *texture);
