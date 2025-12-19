#include "hashmap.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEF_BUCKET_COUNT 30

static void bucket_init(bucket* b) {
	b->value = NULL;
	b->probe = 0;
	b->occupied = 0;
	b->key = NULL;
}

static void bucket_set_v(bucket* b, size_t v_size, void* v) {
	if (b->value == NULL) b->value = malloc(v_size);
	memcpy(b->value, v, v_size);
}

static void bucket_set_k(bucket* b, size_t k_size, void* k) {
	if (b->key == NULL) b->key = malloc(k_size);
	memcpy(b->key, k, k_size);
}

void hashmap_init_detailed(hashmap* m, size_t v_size, size_t k_size, size_t buckets, hashfunc hash) {
	m->buckets = malloc(sizeof(bucket) * buckets);
	for (int i = 0; i < buckets; i++) {
		bucket_init(&m->buckets[i]);
	}
	m->b_count = buckets;
	m->count = 0;
	m->v_size = v_size;
	m->k_size = k_size;
	m->hash = hash;
}

void hashmap_init(hashmap* m, size_t v_size, size_t k_size, hashfunc hash) {
	hashmap_init_detailed(m, v_size, k_size, DEF_BUCKET_COUNT, hash);
}

void hashmap_deinit(hashmap* m) {
	for (int i = 0; i < m->b_count; i++) {
		free(m->buckets[i].value);
	}
	free(m->buckets);
	m->buckets = NULL;
}

void hashmap_put(hashmap* m, void* k, void* v) {
	uint64_t p = m->hash(k) % m->b_count;
	int vpsl = 0;  // probe sequence length
	while (m->buckets[p].occupied == 1) {
		// Collision
		if (memcmp(m->buckets[p].key, k, m->k_size) == 0) {
			bucket_set_v(&m->buckets[p], m->v_size, v);
			bucket_set_k(&m->buckets[p], m->k_size, k);
			m->buckets[p].probe = vpsl;
			m->buckets[p].occupied = 1;
			m->count++;
			return;
		}

		if (vpsl > m->buckets[p].probe) {
			// TODO: swap
		}

		if (vpsl >= m->b_count) {
			// TODO: resize
		}

		p = (p + 1) % m->b_count;
		vpsl++;
	}

	bucket_set_v(&m->buckets[p], m->v_size, v);
	bucket_set_k(&m->buckets[p], m->k_size, k);
	m->buckets[p].probe = vpsl;
	m->buckets[p].occupied = 1;
	m->count++;
}

void* hashmap_get(hashmap* m, void* k) {
	int p = m->hash(k);
	return m->buckets[p].value;
}

void* hashmap_values(hashmap* m) {
	void* values = malloc(m->count * m->v_size);
	int index = 0;
	for (int i = 0; i < m->b_count; i++) {
		if (m->buckets[i].occupied) {
			memcpy(values + index, m->buckets[i].value, m->v_size);
			index++;
		}
		if (index >= m->count - 1) // Every value is taken out
			break;
	}
	return values;
}
