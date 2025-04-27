#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/cJSON.h"

int main() {
    const char *json_string = "{\"name\":\"John\", \"age\":30, \"city\":\"New York\"}";

    cJSON *json = cJSON_Parse(json_string);
    if (json == NULL) {
        printf("Error parsing JSON!\n");
        return 1;
    }

    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    cJSON *age = cJSON_GetObjectItemCaseSensitive(json, "age");
    cJSON *city = cJSON_GetObjectItemCaseSensitive(json, "city");

    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        printf("Name: %s\n", name->valuestring);
    }
    if (cJSON_IsNumber(age)) {
        printf("Age: %d\n", age->valueint);
    }
    if (cJSON_IsString(city) && (city->valuestring != NULL)) {
        printf("City: %s\n", city->valuestring);
    }

    cJSON_Delete(json);

    return 0;
}