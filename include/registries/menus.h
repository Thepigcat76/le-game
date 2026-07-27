#pragma once

#include "../ui.h"

typedef enum {
  MENU_NONE,
  MENU_SAVE,
  MENU_START,
  MENU_BACKPACK,
  MENU_DEBUG,
  MENU_NEW_SAVE,
  MENU_LOAD_SAVE,
  MENU_MAP,
  MENU_DIALOG,
  MENU_INVENTORY,
  MENU_MULTIPLAYER,
  MENU_HOST_SERVER,

  _amount_menu_ids,
} MenuId;

struct _client_game;

typedef void (*MenuOpenFunc)(struct _client_game *);

typedef void (*MenuRenderFunc)(UiRenderer *, struct _client_game *);

typedef struct {
  MenuId id;
  bool container;
  bool hides_game;
  bool pauses_game;

  MenuOpenFunc open_func;
  MenuRenderFunc render_func;

} MenuProperties;
