#include "scenemanager.h"
#include "logger.h"
#include "util/arraylist.h"
#include "rendering/camera.h"
#include "entity.h"
#include "util/hash.h"
#include "util/hashmap.h"
#include "lighting/light.h"
#include "material.h"
#include "core/mesh.h"
#include "scene.h"
#include "util/slotmap.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static SceneManager* sceneman = NULL;

void scenemanager_init(SceneManager* sm) {
	slotmap_init(&sm->scenes, sizeof(scene));
	hashmap_init(&sm->scene_names, SCENE_NAME_MAX, sizeof(uint32_t), str_hash);
	sm->curr_scene_id = 0;
	sceneman = sm;

	sm_create_scene("Default Scene");
}

void scenemanager_deinit(SceneManager* sm) {
	slotmap_deinit(&sm->scenes);
	hashmap_deinit(&sm->scene_names);

	sceneman = NULL;
}

scene* sm_create_scene(char* name) {
	if (sceneman == NULL) {
		LOG(LOG_ERROR, "Cant create scene: Scenemanager not initialized\n");
		return NULL;
	}

	if (hashmap_get_str(&sceneman->scene_names, name) != NULL) {
		LOG(LOG_ERROR, "Scene %s cannot be created: Scene with name already exists\n", name);
		return NULL;
	}

	scene s;
	scene_init(&s, sceneman->scenes.count, name);

	uint32_t id = slotmap_add(&sceneman->scenes, &s);
	hashmap_put_str(&sceneman->scene_names, name, &id); // TODO: fix id stuff here, there exists two different ids
	return slotmap_get(&sceneman->scenes, id);
}

bool sm_load_scene(uint32_t scene_id) {
	if (sceneman == NULL) {
		LOG(LOG_ERROR, "Cant create scene: Scenemanager not initialized\n");
		return false;
	}

	if (scene_id >= sceneman->scenes.count) {
		LOG(LOG_ERROR, "Scene with id %d cant be loaded: Invalid scene id\n", scene_id);
		return false;
	}

	sceneman->curr_scene_id = scene_id;
	return true;
}

bool sm_load_scene_name(char* name) {
	if (sceneman == NULL) {
		LOG(LOG_ERROR, "Cant create scene: Scenemanager not initialized\n");
		return false;
	}

	uint32_t* s_id = hashmap_get_str(&sceneman->scene_names, name);
	if (s_id == NULL) {
		LOG(LOG_ERROR, "Scene %s cant be loaded: It does not exist\n", name);
		return false;
	}
	
	sceneman->curr_scene_id = *s_id;
	return true;
}

scene* sm_get_current_scene() {
	if (sceneman == NULL) {
		LOG(LOG_ERROR, "Cant create scene: Scenemanager not initialized\n");
		return NULL;
	}

	if (sceneman->scenes.count == 0) {
		LOG(LOG_ERROR, "Cant get current scene: There are no scenes\n");
		return NULL;
	}

	return slotmap_get(&sceneman->scenes, sceneman->curr_scene_id);
}

scene* sm_get_scene(char* name) {
	uint32_t* s_id = hashmap_get_str(&sceneman->scene_names, name);
	if (s_id == NULL) return NULL;

	return slotmap_get(&sceneman->scenes, *s_id);
}
