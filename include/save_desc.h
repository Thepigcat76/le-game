#pragma once

#include "../vendor/cJSON.h"

typedef struct {
  char *save_name;
  float seed;
} SaveConfig;

typedef struct {
    int id;
    SaveConfig config;
} SaveDescriptor;

cJSON *save_config_to_json(const SaveConfig *config);
