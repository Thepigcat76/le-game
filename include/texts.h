#pragma once

#include "raylib.h"

// TODO: Implement this

struct text_node {
  char *text;
  Color color;
};

typedef struct {
  struct text_node *nodes;
} FormattedText;
