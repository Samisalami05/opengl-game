#ifndef MESH_H
#define MESH_H

#include <stddef.h>
#include "vec3.h"
#include "vec2.h"

typedef struct vertex {
	vec3 pos;
	vec2 uv;
	vec3 normal;
} vertex;

typedef struct mesh {
	vertex* vertices;
	unsigned int* indices;

	int vertex_count;
	int index_count;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
} mesh;

// --- Mesh ---

mesh* mesh_create(vertex* vertices, int vertex_count, unsigned int* indices, int index_count);
mesh* mesh_load_obj(char* filepath);

void mesh_delete(mesh* m);
void mesh_use(mesh* m);

// --- Vertex ---

void vertex_set_pos(vertex* v, float x, float y, float z);
void vertex_set_uv(vertex* v, float x, float y);
void vertex_set_normal(vertex* v, float x, float y, float z);
void vertex_print(const vertex v);

#endif
