#include "scene.h"
#include "lighting/light.h"
#include "skybox.h"
#include "util/arraylist.h"
#include "util/hashmap.h"
#include <stdio.h>
#include <stdlib.h>

void scene_init(scene* s, unsigned int id, char* name) {
	s->id = id;
	s->name = name;
	camera_init(&s->cam, 800, 800);
	arraylist_init(&s->entities, sizeof(entity));
	arraylist_init(&s->lights, sizeof(light));
	
	mesh* cube_mesh = mesh_load_obj_new("assets/cube.obj");
	material* mat = malloc(sizeof(material));
	material_init(mat, MAT_COLOR_UNLIT);

	entity_init(&s->light_entity, cube_mesh, mat);
	s->light_entity.scale = (vec3){0.5f, 0.5, 0.5f};

	// Create default sun
	light sun = {
		.type = LIGHT_GLOBAL,
		.color = {1.0f, 1.0f, 0.8f},
		.dir = {0.5f, -0.5f, 0.2f},
		.intensity = 0.2f,
	};
	arraylist_append(&s->lights, &sun);

	// Create skybox
	const char* skybox_faces[] = {
		"assets/skybox/right.jpg",
		"assets/skybox/left.jpg",
		"assets/skybox/top.jpg",
		"assets/skybox/bottom.jpg",
		"assets/skybox/front.jpg",
		"assets/skybox/back.jpg",
	};
	skybox_init(&s->skybox, skybox_faces);

}

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
