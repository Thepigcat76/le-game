//! An array of bytes used for saving and loading data to and from
//! binary

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "lilc/alloc.h"
#include "lilc/numbers.h"

#define BYTE_SIZE 8

typedef struct {
  u8 *bytes;
  size_t writer_index;
  size_t reader_index;
  Allocator *allocator;
} ByteBuf;

void byte_buf_init(ByteBuf *buf, Allocator *allocator);

void byte_buf_deinit(ByteBuf *buf);

void byte_buf_write_byte(ByteBuf *buf, u8 byte);

void byte_buf_write_int(ByteBuf *buf, i32 integer);

void byte_buf_write_i64(ByteBuf *buf, i64 integer);

void byte_buf_write_string(ByteBuf *buf, char *str);

u8 byte_buf_read_byte(ByteBuf *buf);

i32 byte_buf_read_int(ByteBuf *buf);

i64 byte_buf_read_i64(ByteBuf *buf);

void byte_buf_read_string(ByteBuf *buf, char *str_buf, int len);

i32 byte_buf_from_file(ByteBuf *buf, const char *name, Allocator *allocator);

void byte_buf_to_file(ByteBuf *buf, const char *name);

#define byte_buf_write_list(buf, func, list, len)                              \
  byte_buf_write_int(buf, len);                                                \
  for (i32 i = 0; i < len; i++) {                                              \
    func(buf, list[i]);                                                        \
  }

#define byte_buf_read_list(buf, func, list)                                    \
  i32 len = byte_buf_read_int(buf);                                        \
  for (i32 i = 0; i < len; i++) {                                              \
    list[i] = func(buf);                                                       \
  }