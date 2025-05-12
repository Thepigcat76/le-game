#pragma once

typedef struct {
    float ambient_light;
    double player_speed;
} Config;

extern Config CONFIG;

void config_on_reload();
