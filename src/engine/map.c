#include "map.h"
#include "allocator.h"
#include "logger.h"
#include "util/hashmap.h"
#include <cstddef>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t bucket_size(Hashmap* map) {
	return map->k_size + map->v_size + sizeof(uint32_t);
}

static uint32_t bucket_key_offset(Hashmap* map, size_t id) {
	return bucket_size(map) * id;
}

static uint32_t bucket_value_offset(Hashmap* map, size_t id) {
	return bucket_key_offset(map, id) + map->k_size;
}

static uint32_t bucket_probe_offset(Hashmap* map, size_t id) {
	return bucket_value_offset(map, id) + map->v_size;
}

static void* bucket_key(Hashmap* map, size_t id) {
	return map->buckets + bucket_key_offset(map, id);
}

static void* bucket_value(Hashmap* map, size_t id) {
	return map->buckets + bucket_value_offset(map, id);
}

static uint32_t* bucket_probe(Hashmap* map, size_t id) {
	return (uint32_t*)(map->buckets + bucket_probe_offset(map, id));
}

void map_init(Hashmap* map, size_t k_size, size_t v_size, HashFunc func) {
	map->k_size = k_size;
	map->v_size = v_size;
	map->hash = func;
}

static void bucket_swap(Hashmap* map, uint32_t a, uint32_t b) {
	// TODO: i think its "capacity - 1"
	void* tmp = map->buckets + (map->capacity - 1) * bucket_size(map);
	memcpy(tmp, bucket_key(map, a), bucket_size(map));
	memcpy(bucket_key(map, a), bucket_key(map, b), bucket_size(map));
	memcpy(bucket_key(map, b), tmp, bucket_size(map));
}

bool map_put(Hashmap* map, void* key, void* value) {
	if (map->count + 2 > map->capacity) { // Keep one bucket at the end for swap
		map->capacity = map->capacity == 0 ? 4 : map->capacity * 2;
		void* tmp = REALLOC(map->buckets, map->capacity * bucket_size(map));
		if (tmp == NULL) {
			LOG(LOG_ERROR, "realloc: %s", strerror(errno));
			return false;
		}
		map->buckets = tmp;
		memset(map->buckets + map->count * bucket_size(map), 0, map->capacity - map->count);
	}

	uint64_t p =  map->hash(key) % map->capacity;
	int vpsl = 0;  // probe sequence length
	while (bucket_key(map, p) != NULL) {
		if (vpsl >= map->capacity) {
			printf("Need to handle this\n");
			return false;
		}

		if (memcmp(bucket_key(map, p), key, map->k_size) == 0) {
			break;
		}

		if (vpsl > *bucket_probe(map, p)) {
		}
	}
}
