#pragma once

#include "lilc/numbers.h"

typedef struct {
  char *input_path;
  char *output_path;
} CliArgs;

void args_parse(CliArgs *args, i32 argc, char **argv);

void args_handle(const CliArgs *args);
