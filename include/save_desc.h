#pragma once

#include "../vendor/cJSON.h"
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

cJSON *save_config_to_json(const SaveConfig *config);
