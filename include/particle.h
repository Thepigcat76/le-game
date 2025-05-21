#pragma once

#include "raylib.h"
#include "shared.h"
#include <stdlib.h>

typedef enum {
  PARTICLE_TILE_BREAK,
} ParticleId;

typedef struct {
  enum {
    PARTICLE_INSTANCE_DEFAULT,
    PARTICLE_INSTANCE_TILE_BREAK,
  } type;
  union {
    Texture2D default_texture;
    struct {
      Texture2D texture;
      Color tint;
    } tile_break;
  } var;
} ParticleInstanceEx;

typedef struct {
  ParticleId id;
  Vector2 position;
  Vector2 velocity;
  float lifetime;
  float age;
  Color color;
  bool active;
  ParticleInstanceEx extra;
} ParticleInstance;

typedef struct {
  ParticleInstance particles[MAX_PARTICLES_AMOUNT];
} ParticleManager;
