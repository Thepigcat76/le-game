#pragma once

#include "../menu.h"
#include "../particle.h"
#include "../shaders.h"
#include "../sounds.h"
#include "../textures.h"
#include "../ui.h"
#include "../window.h"
#include "../keys.h"

typedef struct {
  MenuId menu;
  bool paused;
  // Managers
  TextureManager texture_manager;
  ShaderManager shader_manager;
  SoundManager sound_manager;
  ParticleManager particle_manager;
  UiRenderer ui_renderer;
  Window window;
  RenderTexture2D world_texture;
  bool slot_selected;
  PressedKeys pressed_keys;
} ClientGame;
