#include "../../include/save_desc.h"
#include "../../include/shared.h"
#include "../../vendor/cJSON.h"
#include "lilc/log.h"
#include "lilc/numbers.h"
#include <lilc/alloc.h>
#include <lilc/file.h>
#include <lilc/str.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *adjectives[MAX_SAVE_NAMES_AMOUNT];
  size_t adjectives_amount;
  char *nouns[MAX_SAVE_NAMES_AMOUNT];
  size_t nouns_amount;
} SaveNames;

static SaveNames SAVE_NAMES = {.adjectives_amount = 0, .nouns_amount = 0};

void save_names_on_reload(void) {
  SAVE_NAMES.adjectives_amount = 0;
  SAVE_NAMES.nouns_amount = 0;
  for (int i = 0; i < SAVE_NAMES.adjectives_amount; i++) {
    free(SAVE_NAMES.adjectives[i]);
  }

  for (int i = 0; i < SAVE_NAMES.nouns_amount; i++) {
    free(SAVE_NAMES.nouns[i]);
  }

  dyn_string_t file_content = file_read_to_string("res/data/save_names.json", &HEAP_ALLOCATOR);
  cJSON *json = cJSON_Parse(file_content.string);
  cJSON *adjectives_json = cJSON_GetObjectItemCaseSensitive(json, "adjectives");
  cJSON *nouns_json = cJSON_GetObjectItemCaseSensitive(json, "nouns");
  if (cJSON_IsArray(adjectives_json)) {
    int len = cJSON_GetArraySize(adjectives_json);
    for (int i = 0; i < len; i++) {
      if (i >= MAX_SAVE_NAMES_AMOUNT) {
        break;
      }

      cJSON *elem = cJSON_GetArrayItem(adjectives_json, i);
      if (cJSON_IsString(elem)) {
        char *adjective = malloc(strlen(elem->valuestring) + 1);
        strcpy(adjective, elem->valuestring);
        SAVE_NAMES.adjectives[i] = adjective;
        SAVE_NAMES.adjectives_amount++;
      }
    }
  }

  if (cJSON_IsArray(nouns_json)) {
    int len = cJSON_GetArraySize(nouns_json);
    for (int i = 0; i < len; i++) {
      if (i >= MAX_SAVE_NAMES_AMOUNT) {
        break;
      }

      cJSON *elem = cJSON_GetArrayItem(nouns_json, i);
      if (cJSON_IsString(elem)) {
        char *noun = malloc(strlen(elem->valuestring) + 1);
        strcpy(noun, elem->valuestring);
        SAVE_NAMES.nouns[i] = noun;
        SAVE_NAMES.nouns_amount++;
      }
    }
  }

  dyn_string_free(&file_content);
  cJSON_Delete(json);

  log_info("Reloaded savenames");
}

dyn_string_t generate_save_name(Allocator *allocator) {
  u32 random_adjective = GetRandomValue(0, SAVE_NAMES.adjectives_amount - 1);
  u32 random_noun = GetRandomValue(0, SAVE_NAMES.nouns_amount - 1);

  char *adjective = SAVE_NAMES.adjectives[random_adjective];
  char *noun = SAVE_NAMES.nouns[random_noun];

  dyn_string_t random_name = {0};
  dyn_string_init(&random_name, allocator);

  dyn_string_printf(&random_name, "%s %s", adjective, noun);

  return random_name;
}
