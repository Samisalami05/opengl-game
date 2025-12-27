#include "entity.h"
#include "arraylist.h"
#include "scene.h"
#include "scenemanager.h"

void entity_init(entity* e, mesh* m, material* mat) {
	e->position = (vec3){0, 0, 0};
	e->rotation = (vec3){0, 0, 0};
	e->scale = (vec3){1, 1, 1};
	e->mesh = m;
	e->mat = mat;
}

entity* entity_create(mesh* m, material* mat) {
	entity e;
	entity_init(&e, m, mat);

	scene* s = sm_get_current_scene();
	scene_add_entity(s, &e);
	return arraylist_get_last(&s->entities);
}
