#pragma once

#include "../shared.h"

typedef enum {
  SHADER_TOOLTIP,
  SHADER_LIGHTING,
  _amount_shader_handles,
} ShaderHandle;

extern AssetId SHADER_IDS[_amount_shader_handles];

#define RESOLUTION_LOC 0
