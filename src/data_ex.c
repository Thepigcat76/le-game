#include "../include/data/data_ex.h"
#include <raylib.h>

Vec2i data_map_get_vec2i(const DataMap *data, const char *key) {
    int x = data_map_get(data, TextFormat("%s_x", key)).var.data_int;
    int y = data_map_get(data, TextFormat("%s_y", key)).var.data_int;
    return vec2i(x, y);
}

// TODO: Implement float encoding and decoding
Vec2f data_map_get_vec2f(const DataMap *data, const char *key) {
    int x = data_map_get(data, TextFormat("%s_x", key)).var.data_int;
    int y = data_map_get(data, TextFormat("%s_y", key)).var.data_int;
    return vec2f(x, y);
}

Rectf data_map_get_rectf_static_dimensions(const DataMap *data, const char *key, int width, int height) {
    int x = data_map_get(data, TextFormat("%s_x", key)).var.data_int;
    int y = data_map_get(data, TextFormat("%s_y", key)).var.data_int;
    return rectf(x, y, width, height);
}

void data_map_insert_vec2i(DataMap *data, const char *key, Vec2i vec2i) {
    data_map_insert(data, TextFormat("%s_x", key),  data_int(vec2i.x));
    data_map_insert(data, TextFormat("%s_y", key), data_int(vec2i.y));
}

void data_map_insert_vec2f(DataMap *data, const char *key, Vec2f vec2f) {
    data_map_insert(data, TextFormat("%s_x", key),  data_int(vec2f.x));
    data_map_insert(data, TextFormat("%s_y", key), data_int(vec2f.y));
}

void data_map_insert_rectf_static_dimensions(DataMap *data, const char *key, Rectf rectf) {
    data_map_insert_vec2f(data, key, vec2f(rectf.x, rectf.y));
}
