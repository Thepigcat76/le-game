#pragma once

#include <stdio.h>

#define CONFIG_READ(file_name, json_object_name, block)                                                                \
  char *file = read_file_to_string("config/" file_name ".json");                                                       \
  cJSON *json_object_name = cJSON_Parse(file);                                                                         \
  if (json_object_name == NULL) {                                                                                      \
    printf("Error parsing JSON\n");                                                                                    \
    exit(1);                                                                                                           \
  }                                                                                                                    \
  {block};                                                                                                             \
  cJSON_Delete(json_object_name);                                                                                      \
  free(file);

typedef struct {
  float ambient_light;
  double player_speed;
  int default_font_size;
  int interaction_range;
  float item_pickup_delay;
} Config;

extern Config CONFIG;