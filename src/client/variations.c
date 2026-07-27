#include "../../include/textures.h"
#include "../../vendor/stb_perlin.h"
#include <lilc/alloc.h>
#include <math.h>

void var_tex_load(VariantTexture *var_tex, const cw_Texture *tex, AssetManager *assets) {
  if (!tex->has_meta_info || !tex->meta_info.has_variants) {
    var_tex->kind = TEX_VAR_NONE;
    return;
  }
  var_tex->id = tex->id;
  var_tex->kind = TEX_VAR_SINGLE;
  var_tex->variants = array_new(AssetId, &assets->asset_bump_allocator);

  char **path;
  array_foreach(tex->meta_info.variant_paths, path) { array_add(var_tex->variants, tex_by_tex_path(assets, *path).id); }
}

AssetId var_tex_for_pos(VariantTexture *var_tex, i32 x, i32 y, f32 seed_offset) {
  f32 fx = x * 0.1 + seed_offset;
  f32 fy = y * 0.1 + seed_offset;

  size_t variants = array_len(var_tex->variants);

  f32 raw_noise = stb_perlin_noise3(fx, fy, 0.0f, 0, 0, 0) + 1.0f;
  raw_noise = fminf(1.0f, fmaxf(0.0f, raw_noise));
  f32 noise = raw_noise * (variants - 1);

  return var_tex->variants[(i32)noise];
}
