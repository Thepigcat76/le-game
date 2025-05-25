#pragma once

#define DECLARE_BUTTON_TEXTURE(field_name) field_name, field_name##_SELECTED;

#define INIT_TEXTURE(field_name, texture_name)                                                                         \
  field_name = load_texture("res/assets/gui/" texture_name ".png");                                                    \
  field_name##_SELECTED = load_texture("res/assets/gui/" texture_name "_selected.png")