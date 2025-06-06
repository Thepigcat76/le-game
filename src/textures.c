#include "../include/textures.h"
#include "../include/shared.h"
#include "../vendor/cJSON.h"
#include "raylib.h"
#include <stdlib.h>

AdvTexture adv_texture_load(const char *path) {
  int count;
  const char **text_parts = TextSplit(path, '.', &count);

  const char *anim_file_name = TextFormat("%s.anim", text_parts[0]);
  Texture2D texture = LoadTexture(path);
  if (FileExists(anim_file_name)) {
    int frame_height = texture.height / texture.width;
    int frame_time = 1;
    {
      char *file_content = read_file_to_string(anim_file_name);
      cJSON *json = cJSON_Parse(file_content);
      cJSON *frame_time_json = cJSON_GetObjectItemCaseSensitive(json, "frame_time");
      cJSON *frame_height_json = cJSON_GetObjectItemCaseSensitive(json, "frame_height");
      if (cJSON_IsNumber(frame_time_json)) {
        frame_time = frame_time_json->valueint;
      }
      if (cJSON_IsNumber(frame_height_json)) {
        frame_height = frame_height_json->valueint;
      }
      cJSON_Delete(json);
      free(file_content);
    }
    return (AdvTexture){.type = TEXTURE_ANIMATED,
                        .var = {.texture_animated = {.texture = LoadTexture(path),
                                                     .frame_time = frame_time,
                                                     .frames = texture.height / frame_height}},
                        .width = texture.width,
                        .height = frame_height};
  } else {
    return (AdvTexture){
        .type = TEXTURE_STATIC, .var = {.texture_static = texture}, .width = texture.width, .height = texture.height};
  }
}

void adv_texture_unload(AdvTexture texture) {
  switch (texture.type) {
  case TEXTURE_STATIC: {
    UnloadTexture(texture.var.texture_static);
    break;
  }
  case TEXTURE_ANIMATED:
    UnloadTexture(texture.var.texture_animated.texture);
    break;
  }
}

Texture2D adv_texture_to_texture(AdvTexture texture) {
  switch (texture.type) {
  case TEXTURE_STATIC: {
    return texture.var.texture_static;
  }
  case TEXTURE_ANIMATED: {
    return texture.var.texture_animated.texture;
  }
  }
}
