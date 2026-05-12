#pragma once

#include <stdio.h>
#include "lilc/file.h"

#define CONFIG_READ(file_name, json_object_name, block)                                                                \
  dyn_string_t file = file_read_to_string("config/" file_name ".json", &HEAP_ALLOCATOR);                                                       \
  cJSON *json_object_name = cJSON_Parse(file.string);                                                                         \
  if (json_object_name == NULL) {                                                                                      \
    printf("Error parsing JSON\n");                                                                                    \
    exit(1);                                                                                                           \
  }                                                                                                                    \
  {block};                                                                                                             \
  cJSON_Delete(json_object_name);                                                                                      \
  dyn_string_free(&file);

typedef struct {
  float ambient_light;
  double player_speed;
  int default_font_size;
  int interaction_range;
  float item_pickup_delay;
} Config;

extern Config CONFIG;