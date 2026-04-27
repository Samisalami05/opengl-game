#include "hashmap_str.h"
#include "allocator.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define START_BUCKET_COUNT 16

static void bucket_set_v(bucket_str* b, size_t v_size, void* v) {
	if (b->value == NULL) b->value = MALLOC(v_size);
	memcpy(b->value, v, v_size);
}

static void bucket_set_k(bucket_str* b, char* k) {
	b->key = MALLOC(strlen(k) + 1);
	memcpy(b->key, k, strlen(k) + 1);
}

static uint8_t bucket_occupied(bucket_str* b) {
	return b->value != NULL && b->key != NULL;
}

static void swap(void* a, void* b, size_t size) {
	void* tmp = MALLOC(size);
	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
	FREE(tmp);
}


static uint8_t expand(hashmap_str* m) {
	bucket_str* old = m->buckets;

	size_t prev_count = m->b_count;
	m->b_count = m->b_count <= 0 ? 4 : m->b_count * 2;
	m->buckets = CALLOC(m->b_count, sizeof(bucket_str));
	if (m->buckets == NULL) {
		perror("hashmap: calloc");
		m->b_count /= 2;
		return 1;
	}

	m->count = 0; // Reset count

	for (int i = 0; i < prev_count; i++) {
		void* v = old[i].value;
		void* k = old[i].key;
		hashmap_str_put(m, k, v);
		FREE(v);
		FREE(k);
	}

	FREE(old);
	return 0;
}

void hashmap_str_init_detailed(hashmap_str* m, size_t v_size, size_t buckets, str_hashfunc hash) {
	m->buckets = CALLOC(buckets, sizeof(bucket_str));
	m->b_count = buckets;
	m->count = 0;
	m->v_size = v_size;
	m->hash = hash;
}

void hashmap_str_init(hashmap_str* m, size_t v_size, str_hashfunc hash) {
	hashmap_str_init_detailed(m, v_size, START_BUCKET_COUNT, hash);
}

void hashmap_str_deinit(hashmap_str* m) {
	for (int i = 0; i < m->b_count; i++) {
		bucket_str b = m->buckets[i];
		if (b.value != NULL)
			FREE(b.value);
		if (b.key != NULL)
			FREE(b.key);
	}
	FREE(m->buckets);
	m->buckets = NULL;
}

static void print_bytes(void* v, size_t size) {
	for (int i = 0; i < size; i++) {
		printf("%2x", ((uint8_t*)v)[i]);
	}
	printf("\n");
}

void* hashmap_str_put(hashmap_str* m, const char* k, const void* v) {
	void* value = CALLOC(1, m->v_size);
	memcpy(value, v, m->v_size);
	void* key = CALLOC(1, strlen(k) + 1);
	memcpy(key, k, strlen(k) + 1);

	void* ret = NULL;

	if (m->count == m->b_count) {
		if (expand(m)) {
			FREE(value);
			FREE(key);
			return NULL;
		}
	}

	uint64_t p =  m->hash(key) % m->b_count;
	int vpsl = 0;  // probe sequence length
	while (m->buckets[p].key != NULL && m->buckets[p].value != NULL) {
		if (vpsl >= m->b_count) {
			if (expand(m)) {
				FREE(value);
				FREE(key);
				return NULL;
			}
			p = m->hash(key) % m->b_count;
			vpsl = 0;
			continue;
		}

		bucket_str* bucket = &m->buckets[p];

		// Collision
		if (memcmp(bucket->key, key, strlen(key) + 1) == 0) {
			break;
		}

		if (vpsl > bucket->probe) {
			swap(bucket->value, value, m->v_size);
			char* tmp = bucket->key;
			bucket->key = key;
			key = tmp;

			int tmp_probe = bucket->probe;
			bucket->probe = vpsl;
			vpsl = tmp_probe;
			if (memcmp(bucket->key, key, strlen(key) + 1) == 0) {
				ret = bucket->value;
			}
		}

		p = (p + 1) % m->b_count;
		vpsl++;
	}

	bucket_set_v(&m->buckets[p], m->v_size, value);
	bucket_set_k(&m->buckets[p], key);
	m->buckets[p].probe = vpsl;
	m->count++;

	if (memcmp(m->buckets[p].key, key, strlen(key) + 1) == 0) {
		ret = m->buckets[p].value;
	}

	FREE(value);
	FREE(key);

	return ret;
}

void* hashmap_str_get(hashmap_str* m, const char* k) {
	uint64_t steps = 0;
	uint64_t p = m->hash(k) % m->b_count;
	while (1) {
		bucket_str* bucket = &m->buckets[p];

		if (steps >= m->b_count) {
			return NULL;
		}

		if (bucket_occupied(bucket)) {
			if (memcmp(bucket->key, k, strlen(k) + 1) == 0) {
				break;
			}
		}

		p = (p + 1) % m->b_count;
		steps++;
	}

	return m->buckets[p].value;
}

void* hashmap_str_values(hashmap_str* m) {
	void* values = MALLOC(m->count * m->v_size);
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

char** hashmap_str_keys(hashmap_str* m) {
	char** keys = MALLOC(m->count * sizeof(char*));
	int index = 0;
	for (int i = 0; i < m->b_count; i++) {
		if (m->buckets[i].key != NULL && m->buckets[i].value != NULL) {
			keys[i] = m->buckets[i].key;
			index++;
		}
		if (index >= m->count) // Every value is taken out
			break;
	}
	return keys;
}
