#include "../../include/packed_res_helper.h"
#include "../../include/shared.h"
#include <lilc/log.h>
#include <stdlib.h>

typedef struct {
  const char *path;
  u8 *bytes;
  size_t size;
} ResourceFile;

extern ResourceFile *get_resource_files(size_t *amount);

void resource_files_write(const char *dir_path) {
  size_t amount = 0;
  ResourceFile *files;
#ifdef PACKED_RESOURCES
  files = get_resource_files(&amount);
#endif

  for (size_t i = 0; i < amount; i++) {
    ResourceFile file = files[i];

    const char *path = TextFormat("%s/%s", dir_path, file.path + 4);
    log_debug("File: %s", path);

    ensure_parent_dirs(path, 0755);

    FILE *f = fopen(path, "w");
    if (f == NULL) {
      perror("fopen");
      exit(0);
    }

    fwrite(file.bytes, sizeof(u8), file.size, f);
    fclose(f);
  }
}
