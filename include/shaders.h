#pragma once

#include "raylib.h"
#include <stdlib.h>

#define SHADERS_AMOUNT 3

typedef enum {
  SHADER_LIGHTING,
  SHADER_TOOLTIP_OUTLINE,
  SHADER_TILE_OUTLINE,
} ShaderId;

typedef struct {
  int light_pos_loc;
  int light_color_loc;
  int light_radius_loc;
  int ambient_light_loc;
} ShaderVarLookupLighting;

typedef struct {
} ShaderVarLookupTooltipOutline;

typedef struct {
  ShaderId id;
  union {
    ShaderVarLookupLighting lighting;
    ShaderVarLookupTooltipOutline tooltip_outline;
  } var;
} ShaderVarLookup;

typedef struct {
  Shader shaders[SHADERS_AMOUNT];
  ShaderVarLookup lookups[SHADERS_AMOUNT];
} ShaderManager;

void shaders_load(ShaderManager *manager);

void shaders_unload(ShaderManager *manager);
