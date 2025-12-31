#include "mesh.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include "arraylist.h"
#include "hashmap.h"
#include "ivec3.h"

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

static uint64_t face_hash(const void* v) {
	const ivec3* face = v;
	return ((uint64_t)face->x << 42) | 
		((uint64_t)face->y << 21) |
		(uint64_t)face->z;
}

static void index_print(void* v) {
	printf("%-3d ", *(unsigned int*)v);
}

static void face_print(void * v) {
	ivec3* iv = v;
	printf("(%d, %d, %d) ", iv->x, iv->y, iv->z);
}

static ivec3 read_face(FILE* f) {
	printf("char: %c\n", fgetc(f)); // skip first whitespace
	
	ivec3 out;
	if (fscanf(f, "%d", &out.x) != 1)
		fprintf(stderr, "No vertex found in face\n");
	
	if (fgetc(f) == '/') {
		if (fscanf(f, "%d", &out.y) != 1)
			fprintf(stderr, "No texture coordinate found in face\n");
	} else 
		out.y = -1;

	if (fgetc(f) == '/') {
		if (fscanf(f, "%d", &out.z) != 1)
			fprintf(stderr, "No normal coordinate found in face\n");
	} else 
		out.z = -1;
	printf("%d %d %d\n", out.x, out.y, out.z);

	return out;
}


mesh* mesh_load_obj(char* filepath) {
	FILE* f = fopen(filepath, "rb");
	if (f == NULL) {
		perror("Could not open obj file");
		return NULL;
	}

	hashmap vertex_map;
	hashmap_init(&vertex_map, sizeof(unsigned int), sizeof(ivec3), face_hash);

	arraylist positions, normals, texcoords;
	arraylist_init(&positions, sizeof(vec3));
	arraylist_init(&normals, sizeof(vec3));
	arraylist_init(&texcoords, sizeof(vec2));
	
	arraylist indices, vertices;
	arraylist_init(&indices, sizeof(unsigned int));
	arraylist_init(&vertices, sizeof(vertex));

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
				ivec3 f_id = read_face(f);
				printf("f_id: %d %d %d\n", f_id.x, f_id.y, f_id.z);

				if (f_id.x > (int)positions.count || f_id.y > (int)texcoords.count || f_id.z > (int)normals.count) {
					fprintf(stderr, "Not a valid face format in %s\nvertices: %d, normals: %d, uvs: %d\n", filepath, positions.count, normals.count, texcoords.count);
					return NULL;
				}

				if (hashmap_get(&vertex_map, &f_id) == NULL) {
					vertex v = {
						.pos	= f_id.x >= 0 ? ((vec3*)positions.data)[f_id.x - 1] : (vec3){0, 0, 0},
						.normal = f_id.z >= 0 ? ((vec3*)normals.data)[f_id.z - 1] : (vec3){0, 0, 0},
						.uv     = f_id.y >= 0 ? ((vec2*)texcoords.data)[f_id.y - 1] : (vec2){0, 0},
					};

					unsigned int index = vertices.count;
					//printf("index: %d\n", index);
					//vertex_print(v);
					arraylist_append(&vertices, &v);
					hashmap_put(&vertex_map, &f_id, &index);
				}

				unsigned int* ind = hashmap_get(&vertex_map, &f_id);
				arraylist_append(&indices, ind);
			}
		}
	}

	hashmap_print(&vertex_map, index_print, face_print);
	
	printf("Index count: %d\nVertex count: %d\n", indices.count, vertices.count);

	arraylist_deinit(&positions);
	arraylist_deinit(&normals);
	arraylist_deinit(&texcoords);
	
	hashmap_deinit(&vertex_map);

	for (int i = 0; i < vertices.count; i++) {
		vertex_print(*((vertex*)vertices.data + i));
	}

	for (int i = 0; i < indices.count; i++) {
		printf("%d ", *((unsigned int*)indices.data + i));
	}
	printf("\n");

	return mesh_create((vertex*)vertices.data, vertices.count, (unsigned int*)indices.data, indices.count);
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
