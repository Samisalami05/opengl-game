#include "mesh.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include "arraylist.h"

/* ------------------ Internal Declarations ------------------- */

static void mesh_init(mesh* m);

/* -------------------- Internal Functions -------------------- */

static void mesh_init(mesh* m) {
	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	glGenBuffers(1, &m->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * m->vertex_count, m->vertices, GL_STATIC_DRAW); // TODO: Option to change draw mode
	
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

mesh* mesh_load_obj(char* filepath) {
	FILE* f = fopen(filepath, "rb");
	if (f == NULL) {
		perror("Could not open obj file");
		return NULL;
	}

	arraylist positions, normals, texcoords;
	arraylist_init(&positions, sizeof(vec3));
	arraylist_init(&normals, sizeof(vec3));
	arraylist_init(&texcoords, sizeof(vec2));
	
	unsigned int* indices = NULL;
	int index_count = 0;
	
	char c;
	while ((c = fgetc(f)) != EOF) {
		if (c == 'v') { // Is vertex type
			char c2 = fgetc(f);

			if (c2 == ' ') { // Is vertex pos
				vec3 vpos;
				if (fscanf(f, "%f %f %f", &vpos.x, &vpos.y, &vpos.z) != 3) {
					fprintf(stderr, "Invalid vertex position format in %s\n", filepath);
					return NULL;
				}
				arraylist_append(&positions, &vpos);
			}
			else if (c2 == 'n') { // Is vertex normal
				vec3 vnorm;
				if (fscanf(f, " %f %f %f", &vnorm.x, &vnorm.y, &vnorm.z) != 3) {
					fprintf(stderr, "Invalid vertex normal format in %s\n", filepath);
					return NULL;
				}
				arraylist_append(&normals, &vnorm);
			}
			else if (c2 == 't') { // Is texture coordinate
				vec2 tc;
				if (fscanf(f, " %f %f", &tc.x, &tc.y) != 2) {
					fprintf(stderr, "Invalid vertex normal format in %s\n", filepath);
					return NULL;
				}
				arraylist_append(&texcoords, &tc);
			}
		}
		else if (c == 'f') { // Is face
			for (int i = 0; i < 3; i++) {
				int p_id, t_id, n_id;
				if (fscanf(f, "%d/%d/%d", &p_id, &t_id, &n_id) != 3) break;
				if (p_id > positions.count || n_id > normals.count || t_id > texcoords.count) {
					fprintf(stderr, "Not a valid face format in %s\n", filepath);
					return NULL;
				}
				vertex v = {
					.pos = ((vec3*)positions.data)[p_id - 1],
					.normal = ((vec3*)normals.data)[n_id - 1],
					.uv = ((vec2*)texcoords.data)[t_id - 1],
				};
				//vertex_print(v);

				indices = realloc(indices, sizeof(unsigned int) * (index_count + 1));
				indices[index_count] = p_id - 1;
				index_count++;
			}
		}
	}

	vertex* vertices = malloc(sizeof(vertex) * positions.count);
	for (int i = 0; i < positions.count; i++) {
		vertices[i] = (vertex){
			.pos = ((vec3*)positions.data)[i],
			.normal = (vec3){0, 0, 0},
			.uv = (vec2){0, 0},
		};
		vertex_print(vertices[i]);
	}
	
	printf("Index count: %d\nVertex count: %d\n", index_count, positions.count);

	return mesh_create(vertices, positions.count, indices, index_count);
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

void vertex_print(const vertex v) {
	printf("VERTEX\n");
	printf("pos:    ");
	vec3_print(v.pos);
	printf("norm:   ");
	vec3_print(v.normal);
	printf("uv:     ");
	vec2_print(v.uv);
}
