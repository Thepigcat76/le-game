#include "../../include/game.h"

void game_render_particle(Game *game, ParticleInstance particle, bool behind_player) {
  if (particle.active) {
    switch (particle.extra.type) {
    case PARTICLE_INSTANCE_DEFAULT: {
      if (!behind_player) {
        DrawTexture(particle.extra.var.default_texture, particle.position.x, particle.position.y, WHITE);
      }
      break;
    }
    case PARTICLE_INSTANCE_TILE_BREAK: {
      if (!behind_player) {
        DrawTextureV(particle.extra.var.tile_break.texture, (Vector2){particle.position.x, particle.position.y},
                     particle.color);
      }
      break;
    }
    case PARTICLE_INSTANCE_WALKING: {
      if (game->player->direction == DIRECTION_DOWN && behind_player) {
        DrawTextureV(particle.extra.var.walking.texture, (Vector2){particle.position.x, particle.position.y},
                     particle.color);
      } else if (game->player->direction != DIRECTION_DOWN && !behind_player) {
        DrawTextureV(particle.extra.var.walking.texture, (Vector2){particle.position.x, particle.position.y},
                     particle.color);
      }
      break;
    }
    }
  }
}

static void game_particles_update(Game *game) {
  ParticleInstance *particles = GAME.particle_manager.particles;

  float dt = GetFrameTime();
  for (int i = 0; i < MAX_PARTICLES_AMOUNT; i++) {
    if (particles[i].active) {
      particles[i].position.x += particles[i].velocity.x * dt;
      particles[i].position.y += particles[i].velocity.y * dt;

      // particles[i].velocity.y += 100.0f * dt;

      particles[i].age += dt;
      float alpha = 1.0f - (particles[i].age / particles[i].lifetime);
      // particles[i].color.a = (unsigned char)(255 * alpha);

      if (particles[i].age >= particles[i].lifetime) {
        particles[i].active = false;
      }
    }
  }
}

void game_render_particles(Game *game, bool behind_player) {
  game_particles_update(game);

  ParticleInstance *particles = GAME.particle_manager.particles;
  for (int i = 0; i < MAX_PARTICLES_AMOUNT; i++) {
    game_render_particle(game, particles[i], behind_player);
  }
}