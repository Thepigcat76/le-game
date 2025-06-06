#pragma once

#define DECLARE_BUTTON_TEXTURE(field_name) field_name, field_name##_SELECTED;

#define INIT_TEXTURE(field_name, texture_name)                                                                         \
  field_name = LoadTexture("res/assets/gui/" texture_name ".png");                                                    \
  field_name##_SELECTED = LoadTexture("res/assets/gui/" texture_name "_selected.png")