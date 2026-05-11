#include "packer.h"
#include "lilc/array.h"
#include "lilc/file.h"
#include "lilc/log.h"
#include "shared.h"
#include <dirent.h>
#include <lilc/alloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static void packer_walk_dir(ResourcePacker *packer, const char *path, ResourceFileVisitFunc visit_func) {
  struct dirent *entry;
  DIR *dp = opendir(path);
  if (dp == NULL) {
    perror("opendir");
    fprintf(stderr, "Path: %s\n", path);
    exit(1);
  }

  while ((entry = readdir(dp)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char dir_buf[256];
    sprintf(dir_buf, "%s/%s", path, entry->d_name);
    if (entry->d_type == DT_DIR) {
      packer_walk_dir(packer, dir_buf, visit_func);
      continue;
    } else {
      const char *dot = strrchr(entry->d_name, '.');
      const char *file_ext = NULL;
      if (dot != NULL) {
        file_ext = dot + 1;
      }
      char name[strlen(entry->d_name)];
      size_t length = dot - entry->d_name;
      strncpy(name, entry->d_name, dot - entry->d_name);
      name[length + 1] = '\0';
      FileEntry file_entry = {
          .dir = path,
          .name = entry->d_name,
          .file_ext = file_ext,
          .full_path = dir_buf,
      };
      visit_func(packer, file_entry);
    }
  }

  closedir(dp);
}

static void visit_resource_file(ResourcePacker *packer, FileEntry entry) {
  if (strcmp(entry.file_ext, "aseprite") != 0) {
    array_add(packer->file_paths, strdup(entry.full_path));
  }
}

inline void packer_collect_files(ResourcePacker *packer, const CliArgs *args) {
  packer_walk_dir(packer, args->input_path, visit_resource_file);
}

constexpr char RESOURCES_FILE_HEADER[] =
    "#include <stdlib.h>\n"
    "#include <stdio.h>\n"
    "\n"
    "struct resource_file {\n"
    "  const char *path;\n"
    "  unsigned char *bytes;\n"
    "  size_t size;\n"
    "};\n"
    "\n"
    "static size_t resource_files_amount = 0;\n"
    "static struct resource_file *resource_files = NULL;\n"
    "\n"
    "static void resource_files_make(void);\n"
    "\n"
    "struct resource_file *get_resource_files(size_t *amount) {\n"
    "  if (resource_files == NULL) {\n"
    "    resource_files_make();\n"
    "  }\n"
    "\n"
    "  if (amount != NULL) {"
    "    *amount = resource_files_amount;"
    "  }\n"
    "\n"
    "  return resource_files;\n"
    "}\n"
    "\n"
    "static void resource_file_init(struct resource_file *resource_file, const char *path, size_t size) {\n"
    "  resource_file->path = path;\n"
    "  resource_file->bytes = (unsigned char *) malloc(size);\n"
    "  resource_file->size = size;\n"
    "}\n"
    "\n"
    "static void resource_file_write(struct resource_file *resource_file, size_t *offset, size_t length, long data) {\n"
    "  unsigned long v = (unsigned long)(unsigned long)data;\n"
    "\n"
    "  // bounds check\n"
    "  if (*offset + length > resource_file->size) {\n"
    "    printf(\"Failed to write, out of bounds. Offset: %%zu, Size: %%zu\\n\", *offset, resource_file->size);\n"
    "    exit(1);\n"
    "  }\n"
    "\n"
    "  for (size_t i = 0; i < length; i++) {\n"
    "    resource_file->bytes[*offset + i] = (unsigned char)((v >> i * 8) &0xff);\n"
    "  }\n"
    "\n"
    "  *offset += sizeof(v);\n"
    "}\n"
    "\n"
    "static void resource_files_make(void) {\n"
    "  resource_files = malloc(sizeof(struct resource_file) * %zu);\n"
    "  resource_files_amount = %zu;\n";

constexpr char RESOURCES_FILE_FOOTER[] = "}\n";

constexpr char RESOURCES_FILE_RESOURCE_HEADER[] = "  {\n"
                                                  "    size_t offset = 0;\n"
                                                  "    struct resource_file file = {0};\n"
                                                  "    resource_file_init(&file, \"%s\", %zu);\n";

constexpr char RESOURCES_FILE_RESOURCE_FOOTER[] = "    resource_files[%zu] = file;\n"
                                                  "  }\n";

size_t resource_write_bytes(u8 *bytes, size_t length, size_t offset, FILE *f) {
  u8 *offset_bytes = bytes + offset;

  size_t write_length = length > offset + sizeof(long) ? sizeof(long) : length - offset;

  i64 data = 0;
  for (size_t i = 0; i < write_length; i++) {
    data |= ((unsigned long)offset_bytes[i]) << (8u * i);
  }

  fprintf(f, "    resource_file_write(&file, &offset, %zu, %ld);\n", write_length, data);

  return write_length;
}

static void packer_write_resource(char *file_path, size_t id, FILE *f) {
  log_debug("FILE: %s", file_path);

  FILE *resource_file = fopen(file_path, "r");

  size_t length = 0;
  u8 *bytes = file_read_to_bytes(file_path, &length, &HEAP_ALLOCATOR);

  fprintf(f, RESOURCES_FILE_RESOURCE_HEADER, file_path, length);

  size_t i = 0;
  while (i < length) {
    i += resource_write_bytes(bytes, length, i, f);
  }

  fprintf(f, RESOURCES_FILE_RESOURCE_FOOTER, id);

  free(file_path);
  heap_dealloc(bytes);
}

void packer_write_resources(const ResourcePacker *packer, FILE *f) {
  size_t len = array_len(packer->file_paths);
  fprintf(f, RESOURCES_FILE_HEADER, len, len);

  char **file_path;
  array_foreach(packer->file_paths, file_path) { packer_write_resource(*file_path, _arr_foreach_idx, f); }

  fprintf(f, "%s", RESOURCES_FILE_FOOTER);
}
