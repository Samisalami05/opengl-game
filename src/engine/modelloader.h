#ifndef MODELLOADER_H
#define MODELLOADER_H

#include "core/mesh.h"
#include "material.h"
#include <stdint.h>

typedef struct model {
	mesh* meshes;
	uint32_t mesh_count;
	material* materials;
	uint32_t material_count;
	uint32_t* mesh_mat_indices;
} model;

void load_model(model* m, const char* file);
void model_deinit(model* m);

#endif
