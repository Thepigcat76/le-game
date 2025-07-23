#pragma once

#include <stdbool.h>

#define PRESSED_KEY(id)                                                                                                                    \
  bool id##_key_pressed;                                                                                                                   \
  bool id##_key_down

typedef struct {
  PRESSED_KEY(move_foreward);
  PRESSED_KEY(move_backward);
  PRESSED_KEY(move_left);
  PRESSED_KEY(move_right);
  PRESSED_KEY(open_close_save_menu);
  PRESSED_KEY(open_close_backpack_menu);
  PRESSED_KEY(open_close_debug_menu);
  PRESSED_KEY(close_cur_menu);
  PRESSED_KEY(toggle_hitbox);
  PRESSED_KEY(reload);
  PRESSED_KEY(zoom_in);
  PRESSED_KEY(zoom_out);
  PRESSED_KEY(visit_dungeon);
} PressedKeys;

typedef struct {
  int move_foreward_key;
  int move_backward_key;
  int move_left_key;
  int move_right_key;
  int open_close_save_menu_key;
  int open_close_backpack_menu_key;
  int open_close_debug_menu_key;
  int close_cur_menu_key;
  int toggle_hitbox_key;
  int reload_key;
  int zoom_in_key;
  int zoom_out_key;
  int visit_dungeon_key;
} Keybinds;

extern Keybinds KEYBINDS;
