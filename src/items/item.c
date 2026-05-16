#include "../../include/item.h"
#include "../../include/assets.h"
#include "../../include/data.h"
#include "../../include/data/data_ex.h"
#include "../../include/net/client.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>

void item_render(const ItemInstance *item, int x, int y) {
  RegistryManager registries = CLIENT_GAME.game.registries;
  ItemProperties item_props = registries.items[item->id];

  if (item_props.has_texture) {
    cw_Texture tex = tex_by_id(&CLIENT_GAME.asset_manager, item_props.texture);
    DrawTextureEx(tex.texture, (Vector2){.x = x, .y = y}, 0, 3.5, WHITE);
  }
}

void item_tooltip(const ItemInstance *item, char *buf, size_t buf_capacity) {
  switch (item->id) {
  case ITEM_TORCH: {
    snprintf(buf, buf_capacity, "Le Torch\nLe Sus");
    break;
  }
  default:
    buf[0] = '\0';
    break;
  }
}

void item_save(const ItemInstance *item, DataMap *data, DataContext ctx) { data_map_insert(data, "item", data_int(item->id)); }

void item_load(ItemInstance *item, const DataMap *data, DataContext ctx) {
  ItemId item_id = data_map_get_or_default(data, "item", data_int(ITEM_EMPTY)).var.data_int;
  item->id = item_id;
}

bool item_is_empty(ItemInstance *item) { return item->id == ITEM_EMPTY; }
