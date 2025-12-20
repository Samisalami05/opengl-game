#include "hashmap.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define START_BUCKET_COUNT 16

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

static void swap(void* a, void* b, size_t size) {
	void* tmp = malloc(size);
	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
	free(tmp);
}

static uint8_t expand(hashmap* m) {
	size_t prev_size = m->b_count;
	m->b_count = m->b_count <= 0 ? 4 : m->b_count * m->b_count;
	bucket* tmp = realloc(m->buckets, sizeof(bucket) * m->b_count);
	if (tmp == NULL) {
		perror("hashmap: realloc");
		return 1;
	}
	m->buckets = tmp;

	printf("resizeing from %ld to %ld\n", prev_size, m->b_count);
	for (int i = prev_size; i < m->b_count; i++) {
		printf("%d\n", i);
		bucket_init(&m->buckets[i]);
	}
	printf("resize complete\n");
	
	return 0;
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
	hashmap_init_detailed(m, v_size, k_size, START_BUCKET_COUNT, hash);
}

void hashmap_deinit(hashmap* m) {
	for (int i = 0; i < m->b_count; i++) {
		bucket b = m->buckets[i];
		if (b.value != NULL)
			free(b.value);
		if (b.key != NULL)
			free(b.key);
	}
	free(m->buckets);
	m->buckets = NULL;
}

void hashmap_put(hashmap* m, void* k, void* v) {
	void* value = malloc(m->v_size);
	memcpy(value, v, m->v_size);
	void* key = malloc(m->k_size);
	memcpy(key, k, m->k_size);

	uint64_t p = m->hash(key) % m->b_count;
	int vpsl = 0;  // probe sequence length
	while (m->buckets[p].occupied == 1) {
		bucket* bucket = &m->buckets[p];

		if (vpsl >= m->b_count) {
			printf("Need to resize\n");
			if (expand(m)) {
				free(value);
				free(key);
				return;
			}
		}

		// Collision
		if (memcmp(bucket->key, key, m->k_size) == 0) {
			printf("Found bucket with the same key: bk = %d, k = %d\n", *(int*)bucket->key, *(int*)key);
			bucket_set_v(bucket, m->v_size, value);
			bucket_set_k(bucket, m->k_size, key);
			bucket->probe = vpsl;
			bucket->occupied = 1;
			m->count++;
			return;
		}

		if (vpsl > bucket->probe) {
			printf("swapping\n");
			swap(bucket->value, value, m->v_size);
			swap(bucket->key, key, m->k_size);
			int tmp_probe = bucket->probe;
			bucket->probe = vpsl;
			vpsl = tmp_probe;
		}

		p = (p + 1) % m->b_count;
		vpsl++;
	}

	printf("Found unoccupied bucket at %ld\n", p);
	bucket_set_v(&m->buckets[p], m->v_size, value);
	bucket_set_k(&m->buckets[p], m->k_size, key);
	m->buckets[p].probe = vpsl;
	m->buckets[p].occupied = 1;
	m->count++;

	free(value);
	free(key);
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
			memcpy(values + (index * m->v_size), m->buckets[i].value, m->v_size);
			index++;
		}
		if (index >= m->count) // Every value is taken out
			break;
	}
	return values;
}

void* hashmap_keys(hashmap* m) {
	void* keys = malloc(m->count * m->k_size);
	int index = 0;
	for (int i = 0; i < m->b_count; i++) {
		if (m->buckets[i].occupied) {
			memcpy(keys + (index * m->k_size), m->buckets[i].key, m->k_size);
			index++;
		}
		if (index >= m->count) // Every value is taken out
			break;
	}
	return keys;
}
