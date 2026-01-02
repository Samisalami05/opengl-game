#ifndef ENTITY_H
#define ENTITY_H

#include "material.h"
#include "core/mesh.h"
#include "math/vec3.h"

typedef struct entity {
	vec3 position;
	vec3 rotation;
	vec3 scale; 
	mesh* mesh;
	material* mat;
	unsigned int mat_id;
} entity;

void entity_init(entity* e, mesh* m, material* mat);
entity* entity_create(mesh* m, material* mat);

#endif
