#include "../include/textures.h"
#include "../include/shared.h"
#include "../vendor/cJSON.h"
#include "raylib.h"
#include <stdlib.h>

AnimatedTexture ANIMATED_TEXTURES[ANIMATED_TEXTURES_AMOUNT] = {0};
size_t ANIMATED_TEXTURES_LEN = 0;

AdvTexture adv_texture_load(const char *path) {
  int count;
  const char **text_parts = TextSplit(path, '.', &count);
  Texture2D texture = LoadTexture(path);
  const char *anim_file_name = TextFormat("%s_meta.json", text_parts[0]);
  int frame_height = texture.width;
  int frame_time = 1;
  bool has_anim = false;
  if (FileExists(anim_file_name)) {
    {
      char *file_content = read_file_to_string(anim_file_name);
      cJSON *json = cJSON_Parse(file_content);
      cJSON *animation_json = cJSON_GetObjectItemCaseSensitive(json, "animation");
      if (cJSON_HasObjectItem(json, "animation")) {
        has_anim = true;
        cJSON *frame_time_json = cJSON_GetObjectItemCaseSensitive(animation_json, "frame-time");
        cJSON *frame_height_json = cJSON_GetObjectItemCaseSensitive(animation_json, "frame-height");
        if (cJSON_IsNumber(frame_time_json)) {
          frame_time = frame_time_json->valueint;
        }
        if (cJSON_IsNumber(frame_height_json)) {
          frame_height = frame_height_json->valueint;
        }
      }
      cJSON_Delete(json);
      free(file_content);
    }
  }

  if (has_anim) {
    AdvTexture adv_texture = (AdvTexture){.type = TEXTURE_ANIMATED,
                                          .var = {.texture_animated = {.texture = texture,
                                                                       .animated_texture_id = ANIMATED_TEXTURES_LEN,
                                                                       .frame_time = frame_time,
                                                                       .frames = texture.height / frame_height}},
                                          .path = path,
                                          .width = texture.width,
                                          .height = frame_height};
    ANIMATED_TEXTURES[ANIMATED_TEXTURES_LEN] = (AnimatedTexture){
        .texture = adv_texture, .animated_texture_id = ANIMATED_TEXTURES_LEN, .cur_frame = 0, .frame_timer = 0};
    ANIMATED_TEXTURES_LEN++;
    TraceLog(LOG_INFO, "Loaded animated texture: %s", path);
    return adv_texture;
  } else {
    return (AdvTexture){.type = TEXTURE_STATIC,
                        .var = {.texture_static = texture},
                        .path = path,
                        .width = texture.width,
                        .height = texture.height};
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

int adv_texture_cur_frame(const AdvTexture *texture) {
  if (texture->type != TEXTURE_ANIMATED)
    return 0;
  return ANIMATED_TEXTURES[texture->var.texture_animated.animated_texture_id].cur_frame;
}

int adv_texture_frame_height(const AdvTexture *texture) {
  if (texture->type == TEXTURE_STATIC)
    return texture->var.texture_static.height;
  return texture->var.texture_animated.texture.height / texture->var.texture_animated.frames;
}

Texture2D adv_texture_to_texture(const AdvTexture *texture) {
  switch (texture->type) {
  case TEXTURE_STATIC: {
    return texture->var.texture_static;
  }
  case TEXTURE_ANIMATED: {
    return texture->var.texture_animated.texture;
  }
  }
}
