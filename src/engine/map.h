#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t (*HashFunc)(void*);

typedef struct {
	void* key;
	void* value;
	uint32_t probe;
} Bucket;

typedef struct {
	void* buckets;
	size_t capacity;
	size_t count;
	uint16_t k_size;
	uint16_t v_size;
	HashFunc hash;
} Hashmap;

void map_init(Hashmap* map, size_t k_size, size_t v_size, HashFunc func);
void map_deinit(Hashmap* map);

bool map_insert(Hashmap* map, void* key, void* value);
void* map_get(Hashmap* map, void* key);
void* map_get_cpy(Hashmap* map, void* key);
void map_clear(Hashmap* map);

#endif
