#pragma once

#include "../vendor/cJSON.h"
#include "lilc/str.h"
#include "lilc/alloc.h"
#include <stdbool.h>

typedef struct {
  char *save_name;
  float seed;
} SaveConfig;

typedef struct {
  int id;
  SaveConfig config;
  bool is_server_save;
} SaveDescriptor;

dyn_string_t generate_save_name(Allocator *alloc);

cJSON *save_config_to_json(const SaveConfig *config);
