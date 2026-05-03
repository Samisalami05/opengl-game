#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "scene.h"
#include "util/slotmap.h"
#include <stdint.h>

#define SCENE_NAME_MAX 32

typedef struct {
	slotmap scenes;
	Hashmap scene_names;
	uint32_t curr_scene_id;
} SceneManager;

void scenemanager_init(SceneManager* sm);
void scenemanager_deinit(SceneManager* sm);
scene* sm_create_scene(char* name);
bool sm_load_scene(uint32_t scene_id);
bool sm_load_scene_name(char* name);
scene* sm_get_current_scene();
scene* sm_get_scene(char* name);

#endif
