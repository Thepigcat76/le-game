#include "../../include/textures.h"
#include "lilc/alloc.h"
#include "lilc/array.h"

void animation_manager_load(AnimationManager *anim_manager, const AssetManager *asset_manager) {
  if (anim_manager->animated_textures == NULL) {
    anim_manager->animated_textures = array_new(AnimatedTexture, &HEAP_ALLOCATOR);
  }

  cw_Texture *tex;
  array_foreach(asset_manager->textures, tex) {
    AnimatedTexture anim_tex = {
      .texture = tex->id,
      .width = tex->width,
      .height = tex->height,
      .frame_height = tex->has_meta_info && tex->meta_info.has_animation ? tex->meta_info.animation.frame_height : tex->width,
      .frames = tex->has_meta_info && tex->meta_info.has_animation ? tex->meta_info.animation.frames : 1,

      .cur_frame = 0,
      .frame_timer = 0,
    };
    array_add(anim_manager->animated_textures, anim_tex);
  }

}

void animation_manager_unload(AnimationManager *anim_manager, const AssetManager *asset_manager) {
  array_clear(anim_manager->animated_textures);
}

void animation_manager_tick(AnimationManager *anim_manager, const AssetManager *asset_manager) {
  if (anim_manager->animated_textures == NULL) return;

  AnimatedTexture *anim_tex;
  array_foreach(anim_manager->animated_textures, anim_tex) {
    cw_Texture tex = tex_by_id(asset_manager, anim_tex->texture);
    if (tex.has_meta_info && tex.meta_info.has_animation) {
      anim_tex->frame_timer += TICK_INTERVAL * 1000.0f;
      f32 delay = tex.meta_info.animation.frame_time;
      if (anim_tex->frame_timer >= delay) {
        size_t frames = anim_tex->frames;
        anim_tex->cur_frame = (anim_tex->cur_frame + 1) % frames;
        anim_tex->frame_timer = 0;
      }
    }
  }
}
