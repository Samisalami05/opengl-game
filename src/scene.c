#include "scene.h"
#include "util/arraylist.h"
#include "util/hashmap.h"
#include <stdio.h>

void scene_add_entity(scene* s, entity* e) { // Maybe remove these
	arraylist_append(&s->entities, e);
}

void scene_remove_entity(scene* s, int index) {
	arraylist_remove(&s->entities, index);
}

void scene_print(scene* s) {
	printf("scene: %s\n", s->name);
	printf("id: %d\n", s->id);
	printf("entities: %d\n", s->entities.count);
}
