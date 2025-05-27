#include "../../include/data.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DataMap data_map_new(size_t capacity) {
  return (DataMap){
      .keys = malloc(capacity * sizeof(char *)),
      .values = malloc(capacity * sizeof(Data)),
      .capacity = capacity,
      .len = 0,
  };
}

bool data_map_contains(const DataMap *data_map, char *key) {
  for (size_t i = 0; i < data_map->len; i++) {
    if (strcmp(key, data_map->keys[i]) == 0) {
      return true;
    }
  }
  return false;
}

Data data_map_get(const DataMap *data_map, char *key) {
  for (size_t i = 0; i < data_map->len; i++) {
    if (strcmp(key, data_map->keys[i]) == 0) {
      return data_map->values[i];
    }
  }
  fprintf(stderr, "Data Map does not contain key: %s\n", key);
  exit(EXIT_FAILURE);
}

Data data_map_get_or_default(const DataMap *data_map, char *key,
                             Data default_val) {
  if (data_map_contains(data_map, key)) {
    return data_map_get(data_map, key);
  }
  return default_val;
}

void data_map_insert(DataMap *data_map, char *key, Data val) {
  data_map->keys[data_map->len] =
      malloc(strlen(key) + 1); // allocate enough space
  if (!data_map->keys[data_map->len]) {
    fprintf(stderr, "Failed to allocate memory for key\n");
    exit(1);
  }
  strcpy(data_map->keys[data_map->len], key);
  data_map->values[data_map->len] = val;
  data_map->len++;
}

void byte_buf_write_data_map(ByteBuf *buf, const DataMap *map) {
  byte_buf_write_int(buf, map->len);
  for (int i = 0; i < map->len; i++) {
    byte_buf_write_string(buf, map->keys[i]);
    byte_buf_write_data(buf, &map->values[i]);
  }
}

void byte_buf_read_data_map(ByteBuf *buf, DataMap *map, int len) {
  for (int i = 0; i < len; i++) {
    int key_str_len = byte_buf_read_int(buf);
    char key_buf[key_str_len + 1];
    byte_buf_read_string(buf, key_buf, key_str_len);
    data_map_insert(map, key_buf, byte_buf_read_data(buf));
  }
}

void byte_buf_write_data_list(ByteBuf *buf, const DataList *list) {
  byte_buf_write_int(buf, list->len);
  for (int i = 0; i < list->len; i++) {
    byte_buf_write_data(buf, &list->items[i]);
  }
}

void byte_buf_read_data_list(ByteBuf *buf, DataList *list, int len) {
  for (int i = 0; i < len; i++) {
    list->items[i] = byte_buf_read_data(buf);
  }
}

void byte_buf_write_data(ByteBuf *buf, const Data *data) {
  byte_buf_write_byte(buf, data->type);
  switch (data->type) {
  case DATA_TYPE_BYTE:
    byte_buf_write_byte(buf, data->var.data_byte);
    break;
  case DATA_TYPE_INT:
    byte_buf_write_int(buf, data->var.data_int);
    break;
  case DATA_TYPE_CHAR:
    byte_buf_write_byte(buf, data->var.data_char);
    break;
  case DATA_TYPE_STRING:
    byte_buf_write_string(buf, data->var.data_string);
    break;
  case DATA_TYPE_MAP:
    byte_buf_write_data_map(buf, &data->var.data_map);
    break;
  case DATA_TYPE_LIST:
    byte_buf_write_data_list(buf, &data->var.data_list);
    break;
  default:
    fprintf(stderr, "Error writing data, type: %u\n", data->type);
    break;
  }
}

Data byte_buf_read_data(ByteBuf *buf) {
  uint8_t type = byte_buf_read_byte(buf);

  switch (type) {
  case DATA_TYPE_BYTE: {
    signed char byte = byte_buf_read_byte(buf);
    return (Data){.type = type, .var = {.data_byte = byte}};
  }
  case DATA_TYPE_INT: {
    int32_t integer = byte_buf_read_int(buf);
    return (Data){.type = type, .var = {.data_int = integer}};
  }
  case DATA_TYPE_CHAR: {
    uint8_t character = byte_buf_read_byte(buf);
    return (Data){.type = type, .var = {.data_char = character}};
  }
  case DATA_TYPE_STRING: {
    int len = byte_buf_read_int(buf);
    char *string = malloc(len * sizeof(char));
    byte_buf_read_string(buf, string, len);
    return (Data){.type = type, .var = {.data_string = string}};
  }
  case DATA_TYPE_MAP: {
    size_t len = byte_buf_read_int(buf);
    DataMap map = data_map_new(len);
    byte_buf_read_data_map(buf, &map, len);
    return (Data){.type = type, .var = {.data_map = map}};
  }
  case DATA_TYPE_LIST: {
    size_t len = byte_buf_read_int(buf);
    DataList list = {.items = malloc(len * sizeof(Data)), .len = len};
    byte_buf_read_data_list(buf, &list, len);
    return (Data){.type = type, .var = {.data_list = list}};
  }
  default: {
    fprintf(stderr, "Error reading data, type: %u, wi: %zu, ri: %zu", type, buf->writer_index, buf->reader_index);
    exit(1);
  }
  }
}

Data data_map(DataMap map) {
  return (Data){.type = DATA_TYPE_MAP, .var = {.data_map = map}};
}

Data data_list(DataList list) {
  return (Data){.type = DATA_TYPE_LIST, .var = {.data_list = list}};
}

Data data_byte(signed char c) {
  return (Data){.type = DATA_TYPE_BYTE, .var = {.data_byte = c}};
}

Data data_short(short s) {
  return (Data){.type = DATA_TYPE_SHORT, .var = {.data_short = s}};
}

Data data_int(int i) {
  return (Data){.type = DATA_TYPE_INT, .var = {.data_int = i}};
}

Data data_long(long l) {
  return (Data){.type = DATA_TYPE_LONG, .var = {.data_long = l}};
}

Data data_float(float f) {
  return (Data){.type = DATA_TYPE_FLOAT, .var = {.data_float = f}};
}

Data data_double(double d) {
  return (Data){.type = DATA_TYPE_DOUBLE, .var = {.data_double = d}};
}

Data data_string(char *str) {
  return (Data){.type = DATA_TYPE_STRING, .var = {.data_string = str}};
}

void data_free(Data *data) {
  switch (data->type) {
  case DATA_TYPE_MAP: {
    DataMap *map = &data->var.data_map;
    for (size_t i = 0; i < map->len; i++) {
      free(map->keys[i]);
      data_free(&map->values[i]);
    }
    free(map->keys);
    free(map->values);
    break;
  }
  case DATA_TYPE_STRING: {
    free(data->var.data_string);
    break;
  }
  case DATA_TYPE_LIST: {
    DataList *data_list = &data->var.data_list;
    for (int i = 0; i < data_list->len; i++) {
      data_free(&data_list->items[i]);
    }
    free(data_list->items);
    break;
  }
  default:
    break;
  }
}
