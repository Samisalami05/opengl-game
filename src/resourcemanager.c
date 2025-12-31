#include "resourcemanager.h"
#include "hash.h"
#include "hashmap.h"
#include "shader.h"
#include "texture.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static hashmap texture_map;
static hashmap shader_map;

typedef struct shader_key {
	const char* vertex;
	const char* fragment;
} shader_key;

uint64_t shader_hash(const void* v) {
	const shader_key* sh = v;
	char combined[256];
	strcpy(combined, sh->vertex);
	strcat(combined, sh->fragment);
	uint64_t hash = str_hash(combined);
	return hash;
}

void resource_manager_init() {
	hashmap_init(&texture_map, sizeof(texture*), sizeof(char*), str_hash);
	hashmap_init(&shader_map, sizeof(shader*), sizeof(shader_key), shader_hash);
}

void resource_manager_deinit() {
	hashmap_deinit(&texture_map);
	hashmap_deinit(&shader_map);
}

texture* load_texture(const char* path) {
	texture** stored = hashmap_get(&texture_map, path);
	if (stored != NULL)
		return *stored;
	
	texture* new_tex = malloc(sizeof(texture));
	texture_init(new_tex, path);
	return *(texture**)hashmap_put(&texture_map, path, &new_tex);
}

shader* load_shader(const char* vertex, const char* fragment) {
	printf("loading shader: %s, %s\n", vertex, fragment);
	shader_key key = { vertex, fragment };
	shader** stored = hashmap_get(&shader_map, &key);
	if (stored != NULL) {
		return *stored;
	}

	shader* new_shader = malloc(sizeof(shader));
	shader_init(new_shader, vertex, fragment);
	return *(shader**)hashmap_put(&shader_map, &key, &new_shader);
}
