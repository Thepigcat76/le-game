#pragma once

typedef struct {
    float ambient_light;
    double player_speed;
    int default_font_size;
} Config;

extern Config CONFIG;

void config_on_reload();
