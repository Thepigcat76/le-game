#pragma once

#include "save_desc.h"
#include "space_desc.h"
#include "world.h"

#define SAVE_SPACE_DATA(save_desc, space_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                          \
  {                                                                                                                                        \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                                     \
    ByteBuf byte_buf_name = {.bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};             \
    __VA_ARGS__                                                                                                                            \
    const char *dir_path = TextFormat("save/save%d/spaces/%s-%zu/", save_desc.id, space_id_to_name(space_desc.type->space_id), space_desc.id);\
    dir_create(dir_path);\
    const char *path = TextFormat("save/save%d/spaces/%s-%zu/" save_file_name ".bin", save_desc.id, space_id_to_name(space_desc.type->space_id), space_desc.id);    \
    byte_buf_to_file(&byte_buf_name, path);                                                                                                \
    TraceLog(LOG_INFO, "Saved " save_file_name " at %s data, writer index: %d", path, byte_buf_name.writer_index);                         \
    free(byte_buf_name##_bytes);                                                                                                           \
  }

#define LOAD_SPACE_DATA(save_desc, space_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                          \
  {                                                                                                                                        \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                                     \
    ByteBuf byte_buf_name = {.bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};             \
    const char *path = TextFormat("save/save%d/spaces/%s-%zu/" save_file_name ".bin", save_desc.id, space_id_to_name(space_desc.type->space_id), space_desc.id);    \
    byte_buf_from_file(&byte_buf_name, path);                                                                                              \
    __VA_ARGS__                                                                                                                            \
    TraceLog(LOG_INFO, "Loaded " save_file_name "at %s data, reader index: %d", path, byte_buf_name.reader_index);                                    \
    free(byte_buf_name##_bytes);                                                                                                           \
  }

typedef struct {
  SpaceDescriptor desc;
  World world;
  float seed;
} Space;

void space_create(SpaceDescriptor desc, float seed, Space *space);

void space_create_default(float seed, Space *space);

void space_load(SaveDescriptor save_desc, SpaceDescriptor space_desc, Space *space);

void space_save(SaveDescriptor save_desc, const Space *space);
