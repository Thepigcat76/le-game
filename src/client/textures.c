#include "../../include/textures.h"
#include "../../include/shared.h"
#include <raylib.h>

inline void tex_draw0(cw_Texture *tex, Vec2i pos, Color tint) { tex_draw1(tex, pos, 1.0f, tint); }

inline void tex_draw1(cw_Texture *tex, Vec2i pos, f32 scale, Color tint) {
  tex_draw2(tex, pos, rectf(0, 0, tex->width, tex->height), 0, scale, tint);
}

inline void tex_draw2(cw_Texture *tex, Vec2i dest, Rectf src, f32 rotation, f32 scale, Color tint) {
  tex_draw3(tex, rectf(dest.x, dest.y, (tex->width * scale), (tex->height * scale)), src, vec2f(0, 0),
            rotation, tint);
}

void tex_draw3(cw_Texture *tex, Rectf dest, Rectf src, Vec2f origin, f32 rotation, Color tint) {
  if (tex->has_meta_info && tex->meta_info.is_nine_slice) {
    u32 border = tex->meta_info.nine_slice_border;

    u32 dw = dest.width - border * 2;  // scalable interior width in destination
    u32 dh = dest.height - border * 2; // scalable interior height in destination
    u32 sw = src.width - border * 2;   // scalable interior width in source
    u32 sh = src.height - border * 2;  // scalable interior height in source

    // 1. TOP-LEFT CORNER
    DrawTexturePro(tex->texture, rectf(0, 0, border, border), // src
                   rectf(dest.x, dest.y, border, border),     // dst
                   origin, rotation, tint);

    // 2. TOP EDGE
    DrawTexturePro(tex->texture, rectf(border, 0, sw, border), // src
                   rectf(dest.x + border, dest.y, dw, border), // dst (stretched horizontally)
                   origin, rotation, tint);

    // 3. TOP-RIGHT CORNER
    DrawTexturePro(tex->texture, rectf(border + sw, 0, border, border), // src
                   rectf(dest.x + border + dw, dest.y, border, border), // dst
                   origin, rotation, tint);

    // 4. LEFT EDGE
    DrawTexturePro(tex->texture, rectf(0, border, border, sh), // src
                   rectf(dest.x, dest.y + border, border, dh), // dst (stretched vertically)
                   origin, rotation, tint);

    // 5. CENTER
    DrawTexturePro(tex->texture, rectf(border, border, sw, sh),     // src
                   rectf(dest.x + border, dest.y + border, dw, dh), // dst (stretched both)
                   origin, rotation, tint);

    // 6. RIGHT EDGE
    DrawTexturePro(tex->texture, rectf(border + sw, border, border, sh),     // src
                   rectf(dest.x + border + dw, dest.y + border, border, dh), // dst (stretched vertically)
                   origin, rotation, tint);

    // 7. BOTTOM-LEFT CORNER
    DrawTexturePro(tex->texture, rectf(0, border + sh, border, border), // src
                   rectf(dest.x, dest.y + dh + border, border, border), // dst
                   origin, rotation, tint);

    // 8. BOTTOM EDGE
    DrawTexturePro(tex->texture, rectf(border, border + sh, sw, border),     // src
                   rectf(dest.x + border, dest.y + dh + border, dw, border), // dst (stretched horizontally)
                   origin, rotation, tint);

    // 9. BOTTOM-RIGHT CORNER
    DrawTexturePro(tex->texture, rectf(border + sw, border + sh, border, border),     // src
                   rectf(dest.x + dw + border, dest.y + dh + border, border, border), // dst
                   origin, rotation, tint);
  } else {
    DrawTexturePro(tex->texture, src, dest, origin, rotation, tint);
  }
}