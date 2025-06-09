#include "../../include/item.h"
#include <raylib.h>

static ItemType tool_item(ItemId item_id, char *texture_name, ToolProperties props) {
  return (ItemType){.id = item_id,
                    .texture = LoadTexture(TextFormat("res/assets/%s.png", texture_name)),
                    .tool_properties = props,
                    .is_tool = true};
}

void tool_items_item_init() {
  ItemType type = tool_item(ITEM_PICKAXE, "pickaxe",
                            (ToolProperties){.attack_damage = 0,
                                             .break_speed = 2,
                                             .break_categories = {TILE_CATEGORY_STONE},
                                             .break_categories_amount = 1});
  ITEMS[type.id] = type;
}