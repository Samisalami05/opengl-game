#ifndef SKYBOX_H
#define SKYBOX_H

#include "core/cubemap.h"
#include "core/mesh.h"
#include "core/shader.h"

typedef struct skybox {
	shader* shader;
	cubemap cubemap;
	unsigned int vao;
	unsigned int vbo;
} skybox;

void skybox_init(skybox* sb, const char** textures);
void skybox_deinit(skybox* sb);

#endif
