#include "../../include/assets.h"
#include "../../vendor/cJSON.h"
#include "lilc/log.h"
#include <lilc/alloc.h>
#include <lilc/array.h>
#include <lilc/file.h>
#include <raylib.h>

AssetId SHADER_IDS[_amount_shader_handles];

static i32 shader_uniform_data_type(const char *data_type) {
  if (!data_type) return -1;

  // Scalars
  if (str_eq(data_type, "float")) return SHADER_UNIFORM_FLOAT;
  if (str_eq(data_type, "int"))   return SHADER_UNIFORM_INT;

  // Vectors (float)
  if (str_eq(data_type, "vec2"))  return SHADER_UNIFORM_VEC2;
  if (str_eq(data_type, "vec3"))  return SHADER_UNIFORM_VEC3;
  if (str_eq(data_type, "vec4"))  return SHADER_UNIFORM_VEC4;

  // Vectors (int)
  if (str_eq(data_type, "ivec2")) return SHADER_UNIFORM_IVEC2;
  if (str_eq(data_type, "ivec3")) return SHADER_UNIFORM_IVEC3;
  if (str_eq(data_type, "ivec4")) return SHADER_UNIFORM_IVEC4;

  // Samplers / textures
  // (raylib sets these with SetShaderValueTexture, but the uniform "type" is still SAMPLER2D)
  if (str_eq(data_type, "sampler2d")) return SHADER_UNIFORM_SAMPLER2D;
  if (str_eq(data_type, "sampler2D")) return SHADER_UNIFORM_SAMPLER2D;

  return -1;
}

static void cw_shader_meta_load(const cJSON *json, const cw_Shader *shader, const char *base_path, i32 *locs, size_t *locs_amount) {
  *locs_amount = 0;
  
  cJSON *uniforms = cJSON_GetObjectItem(json, "uniforms");
  
  cJSON *item;
  cJSON_ArrayForEach(item, uniforms) {
    const char *key = item->string;
    const char *val = item->valuestring;

    i32 res = shader_uniform_data_type(val);
    if (res == -1) {
      log_error("Failed to get data type for shader %s. Uniform-name: '%s', Data-type: %s", base_path, key, val);
      return;
    }

    locs[(*locs_amount)++] = GetShaderLocation(shader->shader, key);
  }
}

i32 cw_shader_load(cw_Shader *shader, AssetManager *manager, FileEntry vs_file_entry, FileEntry fs_file_entry, FileEntry meta_file_entry) {
  shader->shader = LoadShader(vs_file_entry.full_path, fs_file_entry.full_path);
  if (vs_file_entry.name != NULL) {
    shader->base_path = str_cpy(vs_file_entry.name, &manager->asset_bump_allocator);
  } else {
    shader->base_path = str_cpy(fs_file_entry.name, &manager->asset_bump_allocator);
  }

  const char *base_path = vs_file_entry.name == NULL ? fs_file_entry.name : vs_file_entry.name;

  if (meta_file_entry.full_path == NULL) {
    log_error("Shader '%s' does not have meta file", base_path);
    return 0;
  }

  dyn_string_t file_content = file_read_to_string(meta_file_entry.full_path, &HEAP_ALLOCATOR);

  cJSON *json = cJSON_Parse(file_content.string);

  cw_shader_meta_load(json, shader, base_path, shader->shader_values_locs, &shader->shader_values_locs_amount);

  cJSON_Delete(json);

  dyn_string_free(&file_content);

  return 1;
}

void cw_shader_unload(cw_Shader *shader) { UnloadShader(shader->shader); }

static AssetId shader_asset_id(AssetManager *m, char *filename) {
  cw_Shader *shader;
  array_foreach(m->shaders, shader) {
    if (str_eq(shader->base_path, filename)) {
      return shader->id;
    }
  }
  return 0;
}

void shader_handles_assign_id(AssetManager *m) {
  SHADER_IDS[SHADER_LIGHTING] = shader_asset_id(m, "lighting");
  SHADER_IDS[SHADER_TOOLTIP] = shader_asset_id(m, "tooltip_outline");
}
