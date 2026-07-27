#include "../include/cutscene.h"
#include "lilc/deque.h"
#include <lilc/alloc.h>

void cut_scene_manager_init(CutSceneManager *manager) {
  manager->cur_scene = CUT_SCENE_NONE;
  deque_init(manager->scenes, &HEAP_ALLOCATOR);
}

void cut_scene_play(CutSceneManager *manager, CutSceneId cutscene) {
  deque_push_back(manager->scenes, cutscene);

  manager->cur_scene = cutscene;
}
