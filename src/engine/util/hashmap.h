/*
 * A hashmap using robin hood hashing. The keys and values are
 * copied and stored in the datastructure itself so no memory
 * manegment is needed by the user.
 */

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef size_t (*HashFunc)(const void*);

typedef struct {
	void* buckets;
	size_t capacity;
	size_t count;
	uint16_t k_size;  // Key size   (in bytes)
	uint16_t v_size;  // Value size (in bytes)
	HashFunc hash;
} Hashmap;

void hashmap_init(Hashmap* map, size_t k_size, size_t v_size, HashFunc func);
void hashmap_deinit(Hashmap* map);

bool hashmap_put(Hashmap* map, void* key, void* value);
void* hashmap_get(Hashmap* map, void* key);
bool hashmap_remove(Hashmap* map, void* key);
void hashmap_clear(Hashmap* map);

void hashmap_keys(Hashmap* map, void* out);
void hashmap_values(Hashmap* map, void* out);

// TODO: implement these
bool hashmap_contains_key(Hashmap* map, void* key);
bool hashmap_contains_value(Hashmap* map, void* value);
void* hashmap_get_cpy(Hashmap* map, void* key);

#endif
