#pragma once

#include "registries/beings.h"
#include "save_desc.h"
#include "space_desc.h"
#include "world.h"
#include "lilc/dir.h"
#include <stdbool.h>

#define SAVE_SPACE_DATA(save_desc, space_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                          \
  {                                                                                                                                        \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                                     \
    ByteBuf byte_buf_name = {.bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};             \
    __VA_ARGS__                                                                                                                            \
    char dir_path[512];                                                                                                                    \
    if (!save_desc.is_server_save) {                                                                                                       \
      strcpy(dir_path,                                                                                                                     \
             TextFormat("save/save%d/spaces/%s-%zu/", save_desc.id, ctx.registries->spaces[space_desc.space_id].ident, space_desc.id));          \
    } else {                                                                                                                               \
      strcpy(dir_path, TextFormat("server-save/spaces/%s-%zu/", ctx.registries->spaces[space_desc.space_id].ident, space_desc.id));              \
    }                                                                                                                                      \
    dir_create(dir_path);                                                                                                                  \
    char path[512];                                                                                                                        \
    if (!save_desc.is_server_save) {                                                                                                       \
      strcpy(path,                                                                                                                         \
             TextFormat("save/save%d/spaces/%s-%zu/" save_file_name ".bin", save_desc.id, ctx.registries->spaces[space_desc.space_id].ident,     \
                        space_desc.id));                                                                                                   \
    } else {                                                                                                                               \
      strcpy(path,                                                                                                                         \
             TextFormat("server-save/spaces/%s-%zu/" save_file_name ".bin", ctx.registries->spaces[space_desc.space_id].ident, space_desc.id));  \
    }                                                                                                                                      \
    byte_buf_to_file(&byte_buf_name, path);                                                                                                \
    TraceLog(LOG_INFO, "Saved " save_file_name " at %s data, writer index: %d", path, byte_buf_name.writer_index);                         \
    free(byte_buf_name##_bytes);                                                                                                           \
  }

#define LOAD_SPACE_DATA(save_desc, space_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                          \
  {                                                                                                                                        \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                                     \
    ByteBuf byte_buf_name = {.bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};             \
    const char *path = TextFormat("save/save%d/spaces/%s-%zu/" save_file_name ".bin", save_desc.id,                                        \
                                  ctx.registries->spaces[space_desc.space_id].ident, space_desc.id);                                             \
    byte_buf_from_file(&byte_buf_name, path);                                                                                              \
    __VA_ARGS__                                                                                                                            \
    TraceLog(LOG_INFO, "Loaded " save_file_name "at %s data, reader index: %d", path, byte_buf_name.reader_index);                         \
    free(byte_buf_name##_bytes);                                                                                                           \
  }

typedef struct {
  SpaceDescriptor desc;

  World world;
  BeingInstance *beings;

  f32 seed;
} Space;

void space_init(Space *space, SpaceDescriptor desc, float seed);

void space_init_default(Space *space, float seed);

void space_add_being(Space *space, BeingInstance being);

void space_remove_being(Space *space, BeingInstance *being);
