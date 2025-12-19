#include "hashmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUCKET_COUNT 30

void hashmap_init(hashmap* m, size_t stride, hashfunc hash) {
	m->buckets = malloc(sizeof(bucket) * BUCKET_COUNT);
	m->count = BUCKET_COUNT;
	m->stride = stride;
	m->hash = hash;
}

void hashmap_put(hashmap* m, void* v) {
	int p = m->hash(v);
	int vpsl = 0;  // probe sequence length
	while (m->buckets[p].value != NULL) {
		p = (p + 1) % m->count;
		vpsl++;
	}

	memcpy(&m->buckets[p], v, m->stride);
	// TODO: assign psl to bucket
}

void* hashmap_get(hashmap* m, void* k);


