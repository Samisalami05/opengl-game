#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "scene.h"

void sm_init();
void sm_deinit();
scene* sm_create_scene(char* name);
void sm_load_scene(int scene_id);
void sm_load_scene_name(char* name);
scene* sm_get_current_scene();
scene* sm_get_scene(char* name);

#endif
