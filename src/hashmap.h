#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdint.h>

typedef uint64_t (*hashfunc)(void*);

typedef struct bucket {
	void* value;
	uint32_t probe : 31; // probe sequence length
	uint32_t occupied : 1;
	void* key;
} bucket;

typedef struct hashmap {
	bucket* buckets;
	size_t b_count;
	size_t count;
	size_t v_size;
	size_t k_size;
	hashfunc hash;
} hashmap;

void hashmap_init(hashmap* m, size_t v_size, size_t k_size, hashfunc hash);
void hashmap_init_detailed(hashmap* m, size_t v_size, size_t k_size, size_t buckets, hashfunc hash);
void hashmap_deinit(hashmap* m);

void hashmap_put(hashmap* m, void* k, void* v);
void* hashmap_get(hashmap* m, void* k);
void* hashmap_values(hashmap* m);

#endif
