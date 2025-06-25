#include "../../include/game.h"

ParticleInstance *game_emit_particle_ex(Game *game, ParticleInstance particle_instance) {
  ParticleInstance *particles = GAME.particle_manager.particles;

  for (int i = 0; i < MAX_PARTICLES_AMOUNT; i++) {
    if (!GAME.particle_manager.particles[i].active) {
      float angle = (float)(rand() % 360) * DEG2RAD;
      float speed = 20 + (rand() % 100);
      game->particle_manager.particles[i] = particle_instance;
      return &game->particle_manager.particles[i];
    }
  }
  return NULL;
}

ParticleInstance *game_emit_particle(Game *game, int x, int y, ParticleId particle_id,
                                     ParticleInstanceEx particle_extra) {
  Vector2 pos = {x, y};
  Color particle_color = WHITE;
  if (particle_extra.type == PARTICLE_INSTANCE_TILE_BREAK) {
    particle_color = particle_extra.var.tile_break.tint;
  } else if (particle_extra.type == PARTICLE_INSTANCE_WALKING) {
    particle_color = particle_extra.var.walking.tint;
  }
  ParticleInstance particle_instance = {.position = pos,
                                        .velocity = vec2f(0, 0),
                                        .lifetime = (1.5f + (float)(rand() % 100) / 1000.0f) / 3,
                                        .age = 0,
                                        .color = particle_color,
                                        .active = true,
                                        .id = particle_id,
                                        .extra = particle_extra};
  return game_emit_particle_ex(game, particle_instance);
}
