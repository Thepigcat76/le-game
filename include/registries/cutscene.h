#pragma once

typedef enum {
  CUT_SCENE_NONE,
  CUT_SCENE_TEST,

  _amount_cut_scene_ids,
} CutSceneId;

typedef struct {
  CutSceneId id;

  char *ident;
} CutSceneProperties;
