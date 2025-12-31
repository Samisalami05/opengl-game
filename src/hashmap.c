#include "hashmap.h"
#include "ivec3.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define START_BUCKET_COUNT 16

static void bucket_set_v(bucket* b, size_t v_size, void* v) {
	if (b->value == NULL) b->value = malloc(v_size);
	memcpy(b->value, v, v_size);
}

static void bucket_set_k(bucket* b, size_t k_size, void* k) {
	if (b->key == NULL) b->key = malloc(k_size);
	memcpy(b->key, k, k_size);
}

static uint8_t bucket_occupied(bucket* b) {
	return b->value != NULL && b->key != NULL;
}

static void swap(void* a, void* b, size_t size) {
	void* tmp = malloc(size);
	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
	free(tmp);
}

static uint8_t expand(hashmap* m) {
	bucket* old = m->buckets;

	size_t prev_count = m->b_count;
	m->b_count = m->b_count <= 0 ? 4 : m->b_count * 2;
	m->buckets = calloc(m->b_count, sizeof(bucket));
	if (m->buckets == NULL) {
		perror("hashmap: calloc");
		m->b_count /= 2;
		return 1;
	}

	m->count = 0; // Reset count

	for (int i = 0; i < prev_count; i++) {
		void* v = old[i].value;
		void* k = old[i].key;
		hashmap_put(m, k, v);
		free(v);
		free(k);
	}

	free(old);
	return 0;
}

void hashmap_init_detailed(hashmap* m, size_t v_size, size_t k_size, size_t buckets, hashfunc hash) {
	m->buckets = calloc(buckets, sizeof(bucket));
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

void* hashmap_put(hashmap* m, const void* k, const void* v) {
	void* value = calloc(1, m->v_size);
	memcpy(value, v, m->v_size);
	void* key = calloc(1, m->k_size);
	memcpy(key, k, m->k_size);
	
	void* ret = NULL;

	if (m->count == m->b_count) {
		if (expand(m)) {
			free(value);
			free(key);
			return NULL;
		}
	}

	uint64_t p =  m->hash(key) % m->b_count;
	int vpsl = 0;  // probe sequence length
	while (m->buckets[p].key != NULL && m->buckets[p].value != NULL) {
		if (vpsl >= m->b_count) {
			if (expand(m)) {
				free(value);
				free(key);
				return NULL;
			}
			p = m->hash(key) % m->b_count;
			vpsl = 0;
			continue;
		}

		bucket* bucket = &m->buckets[p];

		// Collision
		if (memcmp(bucket->key, key, m->k_size) == 0) {
			break;
		}

		if (vpsl > bucket->probe) {
			swap(bucket->value, value, m->v_size);
			swap(bucket->key, key, m->k_size);
			int tmp_probe = bucket->probe;
			bucket->probe = vpsl;
			vpsl = tmp_probe;
			if (memcmp(bucket->key, k, m->k_size) == 0) {
				ret = bucket->value;
			}
		}

		p = (p + 1) % m->b_count;
		vpsl++;
	}

	bucket_set_v(&m->buckets[p], m->v_size, value);
	bucket_set_k(&m->buckets[p], m->k_size, key);
	m->buckets[p].probe = vpsl;
	m->count++;

	if (memcmp(m->buckets[p].key, k, m->k_size) == 0) {
		ret = m->buckets[p].value;
	}

	free(value);
	free(key);

	return ret;
}

void* hashmap_get(hashmap* m, const void* k) {
	uint64_t steps = 0;
	uint64_t p = m->hash(k) % m->b_count;
	while (1) {
		bucket* bucket = &m->buckets[p];

		if (steps >= m->b_count) {
			return NULL;
		}

		if (bucket_occupied(bucket)) {
			if (memcmp(bucket->key, k, m->k_size) == 0) {
				break;
			}
		}

		p = (p + 1) % m->b_count;
		steps++;
	}
	return m->buckets[p].value;
}

void* hashmap_values(hashmap* m) {
	void* values = malloc(m->count * m->v_size);
	int index = 0;
	for (int i = 0; i < m->b_count; i++) {
		if (m->buckets[i].key != NULL && m->buckets[i].value != NULL) {
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
		if (m->buckets[i].key != NULL && m->buckets[i].value != NULL) {
			memcpy(keys + (index * m->k_size), m->buckets[i].key, m->k_size);
			index++;
		}
		if (index >= m->count) // Every value is taken out
			break;
	}
	return keys;
}

void hashmap_print(hashmap* m, void(*v_print)(void*), void(*k_print)(void*)) {
	printf("values: ");
	for (int i = 0; i < m->b_count; i++) {
		if (m->buckets[i].key != NULL && m->buckets[i].value != NULL) {
			v_print(m->buckets[i].value);
		}
	}
	printf("\n");

	printf("keys:   ");
	for (int i = 0; i < m->b_count; i++) {
		if (m->buckets[i].key != NULL) {
			k_print(m->buckets[i].key);
		}
	}
	printf("\n");

	printf("probes: ");
	for (int i = 0; i < m->b_count; i++) {
		if (m->buckets[i].key != NULL && m->buckets[i].value != NULL) {
			printf("%-3d ", m->buckets[i].probe);
		}
	}
	printf("\n");
}
