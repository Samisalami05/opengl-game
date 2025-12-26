#ifndef ENTITY_H
#define ENTITY_H

#include "mesh.h"
#include "vec3.h"

typedef struct entity {
	vec3 position;
	vec3 rotation;
	//vec3 scale; 
	mesh* mesh;
	unsigned int mat_id;
} entity;

void entity_init(entity* e, mesh* m);

#endif
