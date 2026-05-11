#pragma once

#include "assets.h"

typedef struct {
  cw_Texture texture;
  int animated_texture_id;
  int cur_frame;
  float frame_timer;
} AnimatedTexture;

typedef struct{
  AnimatedTexture *textures;
} TextureManager;

i32 cw_tex_cur_frame(const cw_Texture *texture);

i32 cw_tex_frame_height(const cw_Texture *texture);
