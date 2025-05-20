#pragma once

#include "raylib.h"
#include <stdlib.h>

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    float age;
    Color color;
    bool active;
} Particle;


