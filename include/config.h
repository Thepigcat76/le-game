#pragma once

typedef struct {
  float ambient_light;
  double player_speed;
  int default_font_size;
  int interaction_range;
  float item_pickup_delay;
} Config;

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

extern Config CONFIG;

extern Keybinds KEYBINDS;
