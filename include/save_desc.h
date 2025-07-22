#pragma once

typedef struct {
  char *save_name;
  float seed;
} SaveConfig;

typedef struct {
    int id;
    SaveConfig config;
} SaveDescriptor;