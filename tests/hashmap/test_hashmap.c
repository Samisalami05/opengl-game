#include "test_hashmap.h"
#include <hashmap.h>
#include <stdint.h>
#include <stdio.h>
#include "../testing.h"
#include <stdlib.h>

#define INIT_HASHMAP(v_size, k_size, hash) hashmap m; hashmap_init(&m, v_size, k_size, hash);

uint64_t test_hash(void* v) {
	if (v == NULL) return 0;
	int* i = (int*)v;
	return *i;
}

uint64_t probe_hash(void* v) {
	return 0;
}

uint8_t test_hashmap_init() {
	INIT_HASHMAP(sizeof(int), sizeof(int), test_hash);

	TEST_ASSERT(m.buckets != NULL);
	TEST_ASSERT(m.hash != NULL);
	TEST_ASSERT(m.b_count != 0);
	TEST_ASSERT(m.count == 0);
	TEST_ASSERT(m.v_size == sizeof(int));
	TEST_ASSERT(m.k_size == sizeof(int));

	hashmap_deinit(&m);
	return 1;
}

uint8_t test_hashmap_deinit() {
	INIT_HASHMAP(sizeof(int), sizeof(int), test_hash);

	hashmap_deinit(&m);
	TEST_ASSERT(m.buckets == NULL);
	return 1;
}

uint8_t test_hashmap_put() {
	INIT_HASHMAP(sizeof(int), sizeof(uint64_t), test_hash);

	int v = 5;
	uint64_t k = 0;

	hashmap_put(&m, &k, &v);

	int* rv = m.buckets[0].value;	
	TEST_ASSERT(rv != NULL);
	TEST_ASSERT(*rv == 5);

	hashmap_deinit(&m);
	return 1;
}

uint8_t test_hashmap_put_ext() {
	INIT_HASHMAP(sizeof(int), sizeof(uint64_t), test_hash);

	int v = 8;
	uint64_t k = UINT64_MAX;

	hashmap_put(&m, &k, &v);

	int* rv = m.buckets[k % m.b_count].value;	
	TEST_ASSERT(rv != NULL);
	TEST_ASSERT(*rv == v);

	hashmap_deinit(&m);
	return 1;
}

uint8_t test_hashmap_put_probe() {
	INIT_HASHMAP(sizeof(int), sizeof(int), probe_hash);

	for (int i = 0; i < 31; i++) {
		int v = i;
		hashmap_put(&m, &v, &v);
	}

	int* values = hashmap_values(&m);
	int* keys = hashmap_keys(&m);

	for (int i = 0; i < m.count; i++) {
		int v = values[i];
		int k = keys[i];
		TEST_ASSERT(v == i);
		TEST_ASSERT(k == i);
	}

	free(values);
	free(keys);

	hashmap_deinit(&m);
	return 1;
}

uint8_t test_hashmap_put_resize() {
	hashmap m;
	hashmap_init(&m, sizeof(int), sizeof(int), test_hash);

	for (int i = 0; i < 17; i++) {
		int v = i;
		int k = i;
		hashmap_put(&m, &v, &k);
	}

	int* values = hashmap_values(&m);
	int* keys = hashmap_keys(&m);

	for (int i = 0; i < m.count; i++) {
		int v = values[i];
		int k = keys[i];
		TEST_ASSERT(v == i);
		TEST_ASSERT(k == i);
	}

	free(values);
	free(keys);

	hashmap_deinit(&m);
	return 1;
}

uint8_t test_hashmap_get() {
	INIT_HASHMAP(sizeof(int), sizeof(int), test_hash);

	int count = 30;
	int keys[count];
	int values[count];
	for (int i = 0; i < count; i++) {
		keys[i] = rand();
		values[i] = rand();
		hashmap_put(&m, &keys[i], &values[i]);
	}

	for (int i = 0; i < count; i++) {
		int* v = hashmap_get(&m, &keys[i]);
		TEST_ASSERT(*v == values[i]);
	}

	hashmap_deinit(&m);
	return 1;
}

uint8_t test_hashmap() {
	INIT_TEST();

	EXECUTE_SUBTEST(test_hashmap_init);
	EXECUTE_SUBTEST(test_hashmap_deinit);
	EXECUTE_SUBTEST(test_hashmap_put);
	EXECUTE_SUBTEST(test_hashmap_put_ext);
	EXECUTE_SUBTEST(test_hashmap_put_probe);
	EXECUTE_SUBTEST(test_hashmap_put_resize);
	EXECUTE_SUBTEST(test_hashmap_get);

	EXIT_TEST();
}
