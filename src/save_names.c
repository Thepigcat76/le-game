#include "../include/save_names.h"
#include "../include/shared.h"
#include "../vendor/cJSON.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    char *adjectives[MAX_SAVE_NAMES_AMOUNT];
    size_t adjectives_amount;
    char *nouns[MAX_SAVE_NAMES_AMOUNT];
    size_t nouns_amount;
} SaveNames;

static SaveNames SAVE_NAMES = {.adjectives_amount = 0, .nouns_amount = 0};

void save_names_on_reload() {
  for (int i = 0; i < SAVE_NAMES.adjectives_amount; i++) {
    free(SAVE_NAMES.adjectives[i]);
  }

  for (int i = 0; i < SAVE_NAMES.nouns_amount; i++) {
    free(SAVE_NAMES.nouns[i]);
  }

  char *file_content = read_file_to_string("res/data/save_names.json");
  cJSON *json = cJSON_Parse(file_content);
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

  free(file_content);
  cJSON_Delete(json);
}

char *save_names_random_name() {
    int random_adjective = GetRandomValue(0, SAVE_NAMES.adjectives_amount - 1);
    int random_noun = GetRandomValue(0, SAVE_NAMES.nouns_amount - 1);

    char *adjective = SAVE_NAMES.adjectives[random_adjective];
    char *noun = SAVE_NAMES.nouns[random_noun];

    // Adj len + whitspace + Noun len + Null Term
    char *random_name = malloc(strlen(adjective) + 1 + strlen(noun) + 1);
    sprintf(random_name, "%s %s", adjective, noun);

    return random_name;
}
