#include "mesh.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>

/* ------------------ Internal Declarations ------------------- */

static void mesh_init(mesh* m);

/* -------------------- Internal Functions -------------------- */

static void mesh_init(mesh* m) {
	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	glGenBuffers(1, &m->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * m->vertex_count, m->vertices, GL_STATIC_DRAW); // Todo: Option to change draw mode
	
	glGenBuffers(1, &m->ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->index_count * sizeof(unsigned int), m->indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
}

/* ------------------ External Declarations ------------------- */

// --- Mesh ---

mesh* mesh_create(vertex* vertices, int vertex_count, unsigned int* indices, int index_count) {
	mesh* m = malloc(sizeof(mesh));
	m->vertices = vertices;
	m->indices = indices;

	m->vertex_count = vertex_count;
	m->index_count = index_count;
	
	mesh_init(m);

	return m;
}

void mesh_delete(mesh *m) {
	free(m);
}

void mesh_use(mesh *m) {
	glBindVertexArray(m->vao);
}

// --- Vertex ---

void vertex_set_pos(vertex* v, float x, float y, float z) {
	v->pos.x = x;
	v->pos.y = y;
	v->pos.z = z;
}

void vertex_set_uv(vertex* v, float x, float y) {
	v->uv.x = x;
	v->uv.y = y;
}

void vertex_set_normal(vertex* v, float x, float y, float z) {
	v->normal.x = x;
	v->normal.y = y;
	v->normal.z = z;
}
