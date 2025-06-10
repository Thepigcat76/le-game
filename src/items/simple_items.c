#include "../../include/item.h"
#include <stdbool.h>

static inline void simple_item(int item_id, const char *texture_name) {
  Texture2D tex = LoadTexture(TextFormat("res/assets/%s.png", texture_name));
  ITEMS[item_id] = (ItemType){.id = item_id,
                              .texture = tex,
                              .light_source = false,
                              .tool_properties = {.attack_damage = 0, .break_speed = 0, .break_categories = {}},
                              .is_tool = false};
  ITEMS_AMOUNT++;
}

static inline void simple_light_source_item(int item_id, const char *texture_name) {
  Texture2D tex = LoadTexture(TextFormat("res/assets/%s.png", texture_name));
  ITEMS[item_id] = (ItemType){.id = item_id,
                              .texture = tex,
                              .light_source = true,
                              .tool_properties = {.attack_damage = 0, .break_speed = 0, .break_categories = {}},
                              .is_tool = false};
  ITEMS_AMOUNT++;
}

void empty_item_init() {
  ItemType type = {.id = ITEM_EMPTY,
                   .texture = {},
                   .light_source = false,
                   .tool_properties = {.attack_damage = 0, .break_speed = 0},
                   .is_tool = false};
  ITEMS[type.id] = type;
  ITEMS_AMOUNT++;
}

void stick_item_init() {
  simple_item(ITEM_STICK, "stick");
}

void hammer_item_init() {
  simple_item(ITEM_HAMMER, "hammer");
}

void torch_item_init() {
  simple_light_source_item(ITEM_TORCH, "torch");
}

void map_item_init() {
  simple_item(ITEM_MAP, "map");
}

void backpack_item_init() {
  simple_item(ITEM_BACKPACK, "backpack");
}
