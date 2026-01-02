#include "scenemanager.h"
#include "util/arraylist.h"
#include "rendering/camera.h"
#include "entity.h"
#include "util/hash.h"
#include "util/hashmap.h"
#include "lighting/light.h"
#include "material.h"
#include "core/mesh.h"
#include "scene.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static arraylist sm_scenes;
static hashmap sm_scene_names;
static unsigned int sm_curr_scene_id;

void scenemanager_init() {
	arraylist_init(&sm_scenes, sizeof(scene));
	hashmap_init(&sm_scene_names, sizeof(unsigned int), sizeof(char*), str_hash);
	sm_curr_scene_id = 0;

	sm_create_scene("Default Scene");
}

void scenemanager_deinit() {
	arraylist_deinit(&sm_scenes);
	hashmap_deinit(&sm_scene_names);
}

scene* sm_create_scene(char* name) {
	if (hashmap_get(&sm_scene_names, name) != NULL) {
		fprintf(stderr, "scenemanager: Scene %s cannot be created: Scene with name already exists\n", name);
		return NULL;
	}

	scene s;
	scene_init(&s, sm_scenes.count, name);

	arraylist_append(&sm_scenes, &s);
	hashmap_put(&sm_scene_names, name, &s.id);
	return arraylist_get_last(&sm_scenes);
}

void sm_load_scene(int scene_id) {
	if (scene_id < 0 || scene_id >= sm_scenes.count) {
		fprintf(stderr, "scenemanager: Cant load an invalid scene id\n");
		return;
	}

	sm_curr_scene_id = scene_id;
}

void sm_load_scene_name(char* name) {
	unsigned int* s_id = hashmap_get(&sm_scene_names, name);

	if (s_id == NULL) {
		fprintf(stderr, "scenemanager: Scene %s cant be loaded: It does not exist\n", name);
		return;
	}
	
	sm_curr_scene_id = *s_id;
}

scene* sm_get_current_scene() {
	if (sm_scenes.count == 0) {
		fprintf(stderr, "There are no scenes\n");
		return NULL;
	}

	return arraylist_get(&sm_scenes, sm_curr_scene_id);
}

scene* sm_get_scene(char* name) {
	unsigned int* s_id = hashmap_get(&sm_scene_names, name);

	if (s_id == NULL) return NULL;
	return arraylist_get(&sm_scenes, *s_id);
}
