#pragma once

#include "registries/cutscene.h"

typedef struct {
  CutSceneId cur_scene;
  CutSceneId *scenes;
} CutSceneManager;

void cut_scene_manager_init(CutSceneManager *manager);

void cut_scene_play(CutSceneManager *manager, CutSceneId cutscene);
