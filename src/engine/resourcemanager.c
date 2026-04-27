#include "resourcemanager.h"
#include "allocator.h"
#include "util/hash.h"
#include "util/hashmap.h"
#include "core/shader.h"
#include "core/texture.h"
#include "util/hashmap_str.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

static hashmap_str texture_map;
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

static void free_texture_map() {
	bucket_str* buckets = texture_map.buckets;
	for (int i = 0; i < texture_map.b_count; i++) {
		if (buckets[i].value != NULL) {
			texture** tex = buckets[i].value;
			texture_deinit(*tex);
			FREE(*tex);
		}
	}
	hashmap_str_deinit(&texture_map);
}

static void free_shader_map() {
	bucket* buckets = shader_map.buckets;
	for (int i = 0; i < shader_map.b_count; i++) {
		if (buckets[i].value != NULL) {
			shader** shadr = buckets[i].value;
			shader_deinit(*shadr);
			FREE(*shadr);
		}
	}
	hashmap_deinit(&shader_map);
}

void resource_manager_init() {
	hashmap_str_init(&texture_map, sizeof(texture*), str_hash);
	hashmap_init(&shader_map, sizeof(shader*), sizeof(shader_key), shader_hash);
}

void resource_manager_deinit() {
	free_texture_map();
	free_shader_map();
}

texture* load_texture(const char* path) {
	printf("loading texture: %s\n", path);
	texture** stored = hashmap_str_get(&texture_map, path);
	if (stored != NULL)
		return *stored;
	
	texture* new_tex = MALLOC(sizeof(texture));
	texture_init(new_tex, path);
	return *(texture**)hashmap_str_put(&texture_map, path, &new_tex);
}

shader* load_shader(const char* vertex, const char* fragment) {
	printf("loading shader: %s, %s\n", vertex, fragment);
	shader_key key = { vertex, fragment };
	shader** stored = hashmap_get(&shader_map, &key);
	if (stored != NULL) {
		return *stored;
	}

	shader* new_shader = MALLOC(sizeof(shader));
	if (shader_init(new_shader, vertex, fragment)) return NULL;
	return *(shader**)hashmap_put(&shader_map, &key, &new_shader);
}
