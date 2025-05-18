//! A format for game data
#pragma once

#include <stdlib.h>
#include "bytebuf.h"

typedef struct {
  char **keys;
  struct _data *values;
  size_t len;
  size_t capacity;
} DataMap;

typedef struct {
  struct _data *items;
  size_t len;
} DataList;

typedef struct _data {
  enum {
    DATA_TYPE_MAP,
    DATA_TYPE_LIST,
    DATA_TYPE_BYTE,
    DATA_TYPE_SHORT,
    DATA_TYPE_INT,
    DATA_TYPE_LONG,
    DATA_TYPE_CHAR,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_STRING,
  } type;
  union {
    DataMap data_map;
    DataList data_list;
    signed char data_byte;
    short data_short;
    int data_int;
    long data_long;
    char data_char;
    float data_float;
    double data_double;
    char *data_string;
  } var;
} Data;

DataMap data_map_new(size_t capacity);

Data data_map_get(const DataMap *data_map, char *key);

Data data_map_get_or_default(const DataMap *data_map, char *key, Data default_val);

void data_map_insert(DataMap *data_map, char *key, Data val);

void byte_buf_write_data(ByteBuf *buf, const Data *data);

Data byte_buf_read_data(ByteBuf *buf);

Data data_map(DataMap map);

Data data_list(DataList list);

Data data_byte(signed char c);

Data data_short(short s);

Data data_int(int i);

Data data_long(long l);

Data data_float(float f);

Data data_double(double d);

Data data_string(char *str);

void data_free(Data *data);
