#include "../../include/item.h"
#include <raylib.h>

static void tool_item(ItemId item_id, char *texture_name, ToolProperties props) {
  ITEMS[item_id] = (ItemType){.id = item_id,
                              .texture = LoadTexture(TextFormat("res/assets/%s.png", texture_name)),
                              .tool_properties = props,
                              .is_tool = true};
  ITEMS_AMOUNT++;
}

void tool_items_item_init() {
  tool_item(ITEM_PICKAXE, "pickaxe",
                            (ToolProperties){
                                .attack_damage = 0,
                                .break_speed = 2,
                                .break_categories = {.categories = {TILE_CATEGORY_STONE}, .categories_amount = 1},
                            });
  tool_item(ITEM_AXE, "axe",
                            (ToolProperties){
                                .attack_damage = 0,
                                .break_speed = 2,
                                .break_categories = {.categories = {TILE_CATEGORY_WOOD}, .categories_amount = 1},
                            });
}