#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define COMPONENT_MAX_AMOUNT 256

#define _COMPONENT_FIELD(type, name) type name[COMPONENT_MAX_AMOUNT]

#define COMPONENT_CREATE(type, ...)                                                                                    \
  (type) { .val = __VA_ARGS__, .present = true }

#define COMPONENT(name, fields)                                                                                        \
  typedef struct {                                                                                                     \
    struct fields val;                                                                                                 \
    bool present;                                                                                                      \
  } name

#define BUILD_LAYOUT(...)                                                                                              \
  __VA_ARGS__

COMPONENT(PositionComponent, {
  int x;
  int y;
});

typedef void (*ClickAction)();

COMPONENT(ClickActionComponent, { ClickAction action; });

COMPONENT(TextComponent, {
  const char *text;
  int font_size;
  Color font_color;
});

COMPONENT(TextureComponent, { Texture2D texture; });

typedef struct {
  uint32_t next_id;
  _COMPONENT_FIELD(PositionComponent, positions);
  _COMPONENT_FIELD(ClickActionComponent, click_actions);
  _COMPONENT_FIELD(TextComponent, texts);
  _COMPONENT_FIELD(TextureComponent, textures);
} UiECS;

uint32_t ui_create_elem();

void ui_elem_add_pos(uint32_t id, int x, int y);

void ui_elem_add_click_action(uint32_t id, ClickAction action);

void ui_elem_add_text(uint32_t id, const char *text, int font_size, Color font_color);

void ui_elem_add_texture(uint32_t id, Texture2D texture);

void ui_build();

void ui_render();