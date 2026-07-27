#include "../../include/bytebuf.h"
#include "../../include/shared.h"
#include "lilc/alloc.h"
#include "lilc/file.h"
#include <lilc/array.h>
#include <lilc/log.h>
#include <lilc/panic.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

#define BYTEBUF_DEFAULT_CAPACITY 1600000

static bool can_read(const ByteBuf *buf, size_t len) { return buf->reader_index + len <= array_len(buf->bytes); }

void byte_buf_init(ByteBuf *buf, Allocator *allocator) {
  buf->bytes = array_new_capacity(u8, BYTEBUF_DEFAULT_CAPACITY, allocator);
  buf->allocator = allocator;
  buf->reader_index = 0;
  buf->writer_index = 0;
}

void byte_buf_deinit(ByteBuf *buf) {
  if (buf->bytes == NULL)
    return;

  array_free(buf->bytes);
  buf->bytes = NULL;
}

void byte_buf_write_byte(ByteBuf *buf, u8 byte) {
  array_add(buf->bytes, byte);
  buf->writer_index++;
}

void byte_buf_write_int(ByteBuf *buf, i32 integer) {
  for (ssize_t i = (i32)sizeof(i32) - 1; i >= 0; i--) {
    u8 byte = (integer >> (i * 8)) & 0xFF;
    byte_buf_write_byte(buf, byte);
  }
}

void byte_buf_write_i64(ByteBuf *buf, i64 integer) {
  for (ssize_t i = sizeof(i64) - 1; i >= 0; i--) {
    u8 byte = (integer >> (i * 8)) & 0xFF;
    byte_buf_write_byte(buf, byte);
  }
}

void byte_buf_write_string(ByteBuf *buf, char *str) {
  size_t len = strlen(str);
  byte_buf_write_int(buf, (i32)len);
  for (size_t i = 0; i < len; i++) {
    byte_buf_write_byte(buf, str[i]);
  }
}

u8 byte_buf_read_byte(ByteBuf *buf) {
  if (!can_read(buf, sizeof(u8))) {
    panic("Failed to read byte, out of bounds");
  }
  return buf->bytes[buf->reader_index++];
}

static u8 byte_buf_read_byte_unchecked(ByteBuf *buf) {
  return buf->bytes[buf->reader_index++];
}

i32 byte_buf_read_int(ByteBuf *buf) {
  if (!can_read(buf, sizeof(i32))) {
    panic("Failed to read 32-bit integer, out of bounds");
  }

  i32 integer = 0;

  for (size_t i = 0; i < sizeof(i32); i++) {
    u8 byte = byte_buf_read_byte_unchecked(buf);
    integer = (integer << 8) | byte;
  }

  return integer;
}

i64 byte_buf_read_i64(ByteBuf *buf) {
  if (!can_read(buf, sizeof(i64))) {
    panic("Failed to read 64-bit integer, out of bounds");
  }
  i64 integer = 0;

  for (size_t i = 0; i < sizeof(i64); i++) {
    u8 byte = byte_buf_read_byte_unchecked(buf);
    integer = (integer << 8) | byte;
  }

  return integer;
}

void byte_buf_read_string(ByteBuf *buf, char *str_buf, int len) {
  if (!can_read(buf, len)) {
    panic("Failed to read string, out of bounds");
  }

  for (size_t i = 0; i < len; i++) {
    u8 byte = byte_buf_read_byte_unchecked(buf);
    str_buf[i] = byte;
  }
  str_buf[len] = '\0';
}

int byte_buf_to_bin(const ByteBuf *buf, char *str_buf) {
  size_t str_index = 0;
  for (size_t i = 0; i < buf->writer_index; i++) {
    for (ssize_t j = 7; j >= 0; j--) {
      str_buf[str_index++] = (buf->bytes[i] & (1 << j)) ? '1' : '0';
      // printf("byte: %d, str_index: %zu\n", buf->bytes[i], str_index);
    }
  }
  str_buf[str_index] = '\0';
  return str_index;
}

i32 byte_buf_from_file(ByteBuf *buf, const char *name, Allocator *allocator) {
  size_t len = 0;
  u8 *file_content = file_read_to_bytes(name, &len, allocator);
  if (file_content == NULL) {
    log_error("Failed to read file %s", name);
    return 0;
  }

  array_copy_mem(buf->bytes, file_content, len);

  return 1;
}

void byte_buf_to_file(ByteBuf *buf, const char *name) {
  FILE *f = fopen(name, "wb");
  if (!f) {
    panic("Failed to open file %s for writing\n", name);
  }

  log_debug("writer index of bytebuf before saving to file: %zu", buf->writer_index);

  fwrite(buf->bytes, sizeof(u8), array_len(buf->bytes), f);
  fclose(f);

  byte_buf_deinit(buf);
}