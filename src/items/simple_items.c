#include "../../include/item.h"
#include <stdbool.h>

static inline ItemType simple_item(int item_id, const char *texture_name) {
  Texture2D tex = LoadTexture(TextFormat("res/assets/%s.png", texture_name));
  return (ItemType){.id = item_id, .texture = tex, .light_source = false, .tool_properties = {.attack_damage = 0, .break_speed = 0}, .is_tool = false};
}

static inline ItemType simple_light_source_item(int item_id, const char *texture_name) {
  Texture2D tex = LoadTexture(TextFormat("res/assets/%s.png", texture_name));
  return (ItemType){.id = item_id, .texture = tex, .light_source = true, .tool_properties = {.attack_damage = 0, .break_speed = 0}, .is_tool = false};
}

void empty_item_init() {
  ItemType type = {
      .id = ITEM_EMPTY,
      .texture = {},
      .light_source = false,
      .tool_properties = {.attack_damage = 0, .break_speed = 0},
      .is_tool = false
  };
  ITEMS[type.id] = type;
}

void stick_item_init() {
  ItemType type = simple_item(ITEM_STICK, "stick");
  ITEMS[type.id] = type;
}

void hammer_item_init() {
  ItemType type = simple_item(ITEM_HAMMER, "hammer");
  ITEMS[type.id] = type;
}

void torch_item_init() {
  ItemType type = simple_light_source_item(ITEM_TORCH, "torch");
  ITEMS[type.id] = type;
}

void map_item_init() {
  ItemType type = simple_item(ITEM_MAP, "map");
  ITEMS[type.id] = type;
}

void backpack_item_init() {
  ItemType type = simple_item(ITEM_BACKPACK, "backpack");
  ITEMS[type.id] = type;
}
