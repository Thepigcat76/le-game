#pragma once

#include "../data.h"
#include "../shared.h"

Vec2i data_map_get_vec2i(const DataMap *data, const char *key);

Vec2f data_map_get_vec2f(const DataMap *data, const char *key);

Rectf data_map_get_rectf_static_dimensions(const DataMap *data, const char *key, int width, int height);

void data_map_insert_vec2i(DataMap *data, const char *key, Vec2i vec2i);

void data_map_insert_vec2f(DataMap *data, const char *key, Vec2f vec2f);

void data_map_insert_rectf_static_dimensions(DataMap *data, const char *key, Rectf rectf);
