#include "../../include/bytebuf.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>

void byte_buf_write_byte(ByteBuf *buf, uint8_t byte) {
  buf->bytes[buf->writer_index++] = byte;
}

void byte_buf_write_int(ByteBuf *buf, int32_t integer) {
  if (buf->writer_index + sizeof(int32_t) > buf->capacity) {
    return;
  }

  for (int i = sizeof(int32_t) - 1; i >= 0; i--) {
    uint8_t byte = (integer >> (i * 8)) & 0xFF;
    byte_buf_write_byte(buf, byte);
  }
}

void byte_buf_write_string(ByteBuf *buf, char *str) {
  size_t len = strlen(str);
  byte_buf_write_int(buf, (int32_t)len);
  for (int i = 0; i < len; i++) {
    byte_buf_write_byte(buf, str[i]);
  }
}

uint8_t byte_buf_read_byte(ByteBuf *buf) {
  return buf->bytes[buf->reader_index++];
}

int32_t byte_buf_read_int(ByteBuf *buf) {
  if (buf->writer_index < sizeof(int32_t)) {
    return 0;
  }

  int32_t integer = 0;

  for (int i = 0; i < sizeof(int32_t); i++) {
    uint8_t byte = byte_buf_read_byte(buf);
    integer = (integer << 8) | byte;
  }

  return integer;
}

void byte_buf_read_string(ByteBuf *buf, char *str_buf, int len) {
  for (int i = 0; i < len; i++) {
    uint8_t byte = byte_buf_read_byte(buf);
    str_buf[i] = byte;
  }
  str_buf[len] = '\0';
}

int byte_buf_to_bin(const ByteBuf *buf, char *str_buf) {
  size_t str_index = 0;
  for (int i = 0; i < buf->writer_index; i++) {
    for (int j = 7; j >= 0; j--) {
      str_buf[str_index++] = (buf->bytes[i] & (1 << j)) ? '1' : '0';
      //printf("byte: %d, str_index: %zu\n", buf->bytes[i], str_index);
    }
  }
  str_buf[str_index] = '\0';
  return str_index;
}

void byte_buf_from_bin(ByteBuf *buf, const char *str_buf) {
  size_t len = strlen(str_buf) / 8;
  buf->writer_index = len;

  for (int i = 0; i < len; i++) {
    uint8_t byte = 0;
    for (int j = 0; j < BYTE_SIZE; j++) {
      byte = (byte << 1) | (str_buf[i * BYTE_SIZE + j] - '0');
    }
    buf->bytes[i] = byte;
  }
}

void byte_buf_from_file(ByteBuf *buf, const char *name) {
  FILE *f = fopen(name, "rb");
  if (!f) {
    printf("Failed to open file for reading\n");
    exit(1);
  }

  char file_content[buf->capacity * 8 + 1];
  fread(file_content, sizeof(char), sizeof(file_content) - 1, f);
  fclose(f);
  file_content[sizeof(file_content) - 1] = '\0';

  byte_buf_from_bin(buf, file_content);
}

void byte_buf_to_file(const ByteBuf *buf, const char *name) {
  FILE *f = fopen(name, "wb");
  if (!f) {
    printf("Failed to open file for writing\n");
    exit(1);
  }

  TraceLog(LOG_INFO, "writer index: %d", buf->writer_index);

  char bin_str[80000 + 1];
  int size = byte_buf_to_bin(buf, bin_str);

  fwrite(bin_str, sizeof(char), size, f);
  fclose(f);
}