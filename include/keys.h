#pragma once

#include <stdbool.h>

typedef struct {
  bool move_foreward_key;
  bool move_backward_key;
  bool move_left_key;
  bool move_right_key;
  bool zoom_in_key;
  bool zoom_out_key;
} PressedKeys;

typedef struct {
  int move_foreward_key;
  int move_backward_key;
  int move_left_key;
  int move_right_key;
  int open_close_save_menu_key;
  int open_close_backpack_menu_key;
  int open_close_debug_menu_key;
  int close_cur_menu;
  int toggle_hitbox_key;
  int reload_key;
  int zoom_in_key;
  int zoom_out_key;
} Keybinds;

extern Keybinds KEYBINDS;
