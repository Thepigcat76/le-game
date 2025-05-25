#include "../include/ui/ui_components.h"

ButtonClickFunction button_click_simple(void (*on_click_func)()) {
  return (ButtonClickFunction){
      .func_type = BUTTON_CLICK_FUNC_NO_ARGS, .func_var = {.on_click_no_args = on_click_func}, .args = NULL};
}

ButtonClickFunction button_click_args(void (*on_click_func)(void *), void *args) {
  return (ButtonClickFunction){
      .func_type = BUTTON_CLICK_FUNC_WITH_ARGS, .func_var = {.on_click_with_args = on_click_func}, .args = args};
}