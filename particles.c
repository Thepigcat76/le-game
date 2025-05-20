#include "raylib.h"
#include <stdlib.h> // For rand()
#include <math.h>   // For cosf/sinf

#define MAX_PARTICLES 500

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    float age;
    Color color;
    bool active;
} Particle;

Particle particles[MAX_PARTICLES];

// Emit a new particle at a position
void EmitParticle(Vector2 pos) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            float angle = (float)(rand() % 360) * DEG2RAD;
            float speed = 50 + (rand() % 100);

            particles[i].position = pos;
            particles[i].velocity = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
            particles[i].lifetime = 1.5f + (float)(rand() % 100) / 100.0f;
            particles[i].age = 0;
            particles[i].color = WHITE;
            particles[i].active = true;
            break;
        }
    }
}

// Update all active particles
void UpdateParticles(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].position.x += particles[i].velocity.x * dt;
            particles[i].position.y += particles[i].velocity.y * dt;

            particles[i].velocity.y += 100.0f * dt; // gravity

            particles[i].age += dt;
            float alpha = 1.0f - (particles[i].age / particles[i].lifetime);
            particles[i].color.a = (unsigned char)(255 * alpha);

            if (particles[i].age >= particles[i].lifetime) {
                particles[i].active = false;
            }
        }
    }
}

// Draw all active particles
void DrawParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            DrawCircleV(particles[i].position, 3, particles[i].color);
        }
    }
}

int main() {
    InitWindow(800, 600, "Raylib Particle System");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            for (int i = 0; i < 5; i++) {
                EmitParticle(GetMousePosition());
            }
        }

        UpdateParticles(dt);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawParticles();
        DrawText("Hold Left Click to Emit Particles", 10, 10, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}