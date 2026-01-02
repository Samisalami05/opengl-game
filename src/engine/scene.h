#ifndef SCENE_H
#define SCENE_H

#include "skybox.h"
#include "util/arraylist.h"
#include "entity.h"
#include "rendering/camera.h"
#include "util/hashmap.h"
#include <stddef.h>

typedef struct scene {
	unsigned int id;
	char* name;
	arraylist entities;
	arraylist lights;
	entity light_entity;
	camera cam;
	skybox skybox;
} scene;

void scene_init(scene* s, unsigned int id, char* name);
void scene_add_entity(scene* s, entity* e);
void scene_remove_entity(scene* s, int index);
void scene_print(scene* s);

#endif
