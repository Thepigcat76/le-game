#include "cli.h"
#include "lilc/array.h"
#include "lilc/log.h"
#include "packer.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define NEXT_ARG(i, argc)                                                                                                                  \
  if (i + 1 < argc)                                                                                                                        \
    i++;                                                                                                                                   \
  else                                                                                                                                     \
    break

void args_parse(CliArgs *args, i32 argc, char **argv) {
  if (argc == 1) {
    log_error("[RESOURCE-PACKER] Input directory needs to be provided as first argument");
    exit(0);
  }

  if (args->input_path == NULL) {
    args->output_path = "packed-res/out.c";
  }

  i32 i = 1;
  while (i < argc) {
    if (i == 1) {
      args->input_path = argv[i];
    }

    if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 >= argc) {
        log_error("[RESOURCE-PACKER] Option '-o' requires an output file");
        exit(0);
      }

      NEXT_ARG(i, argc);

      args->output_path = argv[i];
    }

    NEXT_ARG(i, argc);
  }
}

static void args_validate(const CliArgs *args) {
  DIR *d = opendir(args->input_path);

  if (d == NULL) {
    log_error("[RESOURCE-PACKER] Unable to find specified input directory %s", args->input_path);
    exit(0);
  }
  closedir(d);
}

static i32 ensure_parent_dirs(const char *filepath, mode_t mode) {
  char dir[PATH_MAX];
  if (snprintf(dir, sizeof dir, "%s", filepath) >= (int)sizeof dir) {
    errno = ENAMETOOLONG;
    return -1;
  }

  char *slash = strrchr(dir, '/');
  if (!slash)
    return 0; // no directory component
  if (slash == dir)
    return 0; // parent is "/"

  *slash = '\0'; // keep just the directory part
  if (!*dir) {
    errno = EINVAL;
    return -1;
  }

  char tmp[PATH_MAX];
  if (snprintf(tmp, sizeof tmp, "%s", dir) >= (int)sizeof tmp) {
    errno = ENAMETOOLONG;
    return -1;
  }

  size_t len = strlen(tmp);
  if (len > 1 && tmp[len - 1] == '/')
    tmp[len - 1] = '\0';

  for (char *p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0';
      if (mkdir(tmp, mode) != 0 && errno != EEXIST)
        return -1;
      *p = '/';
    }
  }
  if (mkdir(tmp, mode) != 0 && errno != EEXIST)
    return -1;
  return 0;
}

void args_handle(const CliArgs *args) {
  args_validate(args);

  ResourcePacker packer = {.file_paths = array_new(char *, &HEAP_ALLOCATOR)};

  packer_collect_files(&packer, args);

  ensure_parent_dirs(args->output_path, 0755);

  FILE *f = fopen(args->output_path, "w");

  packer_write_resources(&packer, f);

  fclose(f);
  array_free(packer.file_paths);
}
