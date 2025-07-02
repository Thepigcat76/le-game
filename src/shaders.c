#include "../include/shaders.h"
#include "../include/net/client.h"
#include <raylib.h>
#include <stdbool.h>

static const char *VERTEX_SHADER_NAMES[] = {NULL, NULL, "res/shaders/tile_outline.vs"};
static const char *FRAGMENT_SHADER_NAMES[] = {"res/shaders/lighting.fs", "res/shaders/tooltip_outline.fs",
                                              "res/shaders/tile_outline.fs"};

static void shader_load(ShaderManager *manager, ShaderId id) {
  manager->shaders[id] = LoadShader(VERTEX_SHADER_NAMES[id], FRAGMENT_SHADER_NAMES[id]);
  switch (id) {
  case SHADER_LIGHTING: {
    manager->lookups[id].var.lighting = (ShaderVarLookupLighting){
        .light_pos_loc = GetShaderLocation(manager->shaders[id], "light_pos"),
        .light_color_loc = GetShaderLocation(manager->shaders[id], "light_color"),
        .light_radius_loc = GetShaderLocation(manager->shaders[id], "light_radius"),
        .ambient_light_loc = GetShaderLocation(manager->shaders[id], "ambient_light"),
    };
    break;
  }
  case SHADER_TOOLTIP_OUTLINE: {
    manager->lookups[id].var.tooltip_outline = (ShaderVarLookupTooltipOutline){};
    break;
  }
  case SHADER_TILE_OUTLINE: {
    manager->lookups[id].var.tooltip_outline = (ShaderVarLookupTooltipOutline){};
    break;
  }
  }
}

void shaders_load(ShaderManager *manager) {
  for (int i = 0; i < SHADERS_AMOUNT; i++) {
    shader_load(manager, i);
  }
}

static void shader_unload(ShaderManager *manager, ShaderId id) { UnloadShader(manager->shaders[id]); }

void shaders_unload(ShaderManager *manager) {
  for (int i = 0; i < SHADERS_AMOUNT; i++) {
    shader_unload(manager, i);
  }
}

static bool initial_load = true;

void shaders_on_reload(ClientGame *game) {
  if (initial_load) {
    initial_load = false;
  } else {
    shaders_unload(&game->shader_manager);
  }

  shaders_load(&game->shader_manager);
}
