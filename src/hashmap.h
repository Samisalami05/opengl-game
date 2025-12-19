#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdint.h>

typedef uint64_t (*hashfunc)(void*);

typedef struct bucket {
	void* value;
	int psl : 31;
	int occupied : 1;
} bucket;

typedef struct hashmap {
	bucket* buckets;
	size_t count;
	size_t stride;
	hashfunc hash;
} hashmap;

void hashmap_init(hashmap* m, size_t stride, hashfunc hash);
void hashmap_put(hashmap* m, void* v);
void* hashmap_get(hashmap* m, void* k);

#endif
