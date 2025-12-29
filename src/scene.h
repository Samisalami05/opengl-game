#ifndef SCENE_H
#define SCENE_H

#include "arraylist.h"
#include "entity.h"
#include "camera.h"
#include "hashmap.h"
#include <stddef.h>

typedef struct scene {
	unsigned int id;
	char* name;
	arraylist entities;
	arraylist lights;
	entity light_entity;
	camera cam;
} scene;

void scene_add_entity(scene* s, entity* e);
void scene_remove_entity(scene* s, int index);
void scene_print(scene* s);

#endif
