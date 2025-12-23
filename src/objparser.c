#include "arraylist.h"
#include "hashmap.h"
#include "ivec3.h"
#include "mesh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t face_hash(void* v) {
	ivec3* face = v;
	return ((uint64_t)face->x << 42) | 
		((uint64_t)face->y << 21) |
		(uint64_t)face->z;
}

ivec3 parse_obj_index(char* tok) {
    ivec3 idx = {0, 0, 0};

    char* p = tok;
    idx.x = strtol(p, &p, 10);

    if (*p == '/') {
        p++;
        if (*p != '/') {
            idx.y = strtol(p, &p, 10);
        }
        if (*p == '/') {
            p++;
            idx.z = strtol(p, &p, 10);
        }
    }

    return idx;
}

void add_vertex(
    ivec3 idx,
    arraylist* positions,
    arraylist* texcoords,
    arraylist* normals,
    arraylist* vertices,
    arraylist* indices,
    hashmap* vert_map
) {
    unsigned int* found = hashmap_get(vert_map, &idx);

    if (!found) {
        vertex v = {0};

        v.pos = ((vec3*)positions->data)[idx.x - 1];
        if (idx.y) v.uv = ((vec2*)texcoords->data)[idx.y - 1];
        if (idx.z) v.normal = ((vec3*)normals->data)[idx.z - 1];

        unsigned int new_index = vertices->count;
        arraylist_append(vertices, &v);
        hashmap_put(vert_map, &idx, &new_index);
        arraylist_append(indices, &new_index);
    } else {
        arraylist_append(indices, found);
    }
}

void parse_face(
    char* str,
    arraylist* positions,
    arraylist* texcoords,
    arraylist* normals,
    arraylist* vertices,
    arraylist* indices,
    hashmap* vert_map
) {
    ivec3 face[4];
    int count = 0;

    char* tok = strtok(str, " \t\n");
    while (tok && count < 4) {
        face[count++] = parse_obj_index(tok);
        tok = strtok(NULL, " \t\n");
    }

    // triangulate fan: (0, i, i+1)
    for (int i = 1; i + 1 < count; i++) {
        add_vertex(face[0], positions, texcoords, normals, vertices, indices, vert_map);
        add_vertex(face[i], positions, texcoords, normals, vertices, indices, vert_map);
        add_vertex(face[i + 1], positions, texcoords, normals, vertices, indices, vert_map);
    }
}


mesh* mesh_load_obj_new(char* filepath) {
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

	char line[512];

	while (fgets(line, sizeof(line), f)) {

		// Vertex position
		if (strncmp(line, "v ", 2) == 0) {
			vec3 v;
			sscanf(line + 2, "%f %f %f", &v.x, &v.y, &v.z);
			arraylist_append(&positions, &v);
		}

		// Normal
		else if (strncmp(line, "vn ", 3) == 0) {
			vec3 n;
			sscanf(line + 3, "%f %f %f", &n.x, &n.y, &n.z);
			arraylist_append(&normals, &n);
		}

		// Texcoord
		else if (strncmp(line, "vt ", 3) == 0) {
			vec2 uv;
			sscanf(line + 3, "%f %f", &uv.x, &uv.y);
			arraylist_append(&texcoords, &uv);
		}

		// Face
		else if (strncmp(line, "f ", 2) == 0) {
			parse_face(
				line + 2,
				&positions, &texcoords, &normals,
				&vertices, &indices, &vertex_map
			);
		}
	}
	
	return mesh_create((vertex*)vertices.data, vertices.count, (unsigned int*)indices.data, indices.count);
}
