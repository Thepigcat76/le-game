#include "../../include/data.h"
#include "../../vendor/cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static cJSON *data_reader_data_to_json(Data *data) {
  switch (data->type) {
  case DATA_TYPE_BYTE: {
    return cJSON_CreateNumber(data->var.data_byte);
  }
  case DATA_TYPE_SHORT: {
    return cJSON_CreateNumber(data->var.data_short);
  }
  case DATA_TYPE_INT: {
    return cJSON_CreateNumber(data->var.data_int);
  }
  case DATA_TYPE_LONG: {
    return cJSON_CreateNumber(data->var.data_long);
  }
  case DATA_TYPE_FLOAT: {
    return cJSON_CreateNumber(data->var.data_float);
  }
  case DATA_TYPE_DOUBLE: {
    return cJSON_CreateNumber(data->var.data_double);
  }
  case DATA_TYPE_CHAR: {
    return cJSON_CreateString(data->var.data_string);
  }
  case DATA_TYPE_STRING: {
    return cJSON_CreateString(data->var.data_string);
  }
  case DATA_TYPE_MAP: {
    cJSON *map_obj = cJSON_CreateObject();
    DataMap map = data->var.data_map;
    for (int i = 0; i < map.len; i++) {
      cJSON *elem_json = data_reader_data_to_json(&map.values[i]);
      cJSON_AddItemToObject(map_obj, map.keys[i], elem_json);
    }
    return map_obj;
  }
  case DATA_TYPE_LIST: {
    cJSON *list_obj = cJSON_CreateArray();
    DataList list = data->var.data_list;
    for (int i = 0; i < list.len; i++) {
      cJSON *elem_json = data_reader_data_to_json(&list.items[i]);
      cJSON_AddItemToArray(list_obj, elem_json);
    }
    return list_obj;
  }
  }
}

char *data_reader_read_data(Data *data) {
  cJSON *json = data_reader_data_to_json(data);
  char *str = cJSON_Print(json);  // pretty-printed with tabs
  cJSON_Delete(json);

  // Optional: convert tabs to 2 spaces for VSCode-style formatting
  for (char *p = str; *p; ++p) {
    if (*p == '\t') *p = ' ';  // Replace tabs with space (you can add more if needed)
  }

  FILE *f = fopen("debug_data.json", "w");

  fprintf(f, "%s", str);

  fclose(f);

  return str;
}
