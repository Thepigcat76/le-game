#pragma once

#include "shared.h"
#include <stdio.h>
#include "cli.h"

typedef struct {
  char **file_paths;
} ResourcePacker;

typedef void (*ResourceFileVisitFunc)(ResourcePacker *, FileEntry);

void packer_collect_files(ResourcePacker *packer, const CliArgs *args);

void packer_write_resources(const ResourcePacker *packer, FILE *f);
