#include "test_hashmap.h"
#include <hashmap.h>
#include <stdint.h>
#include <stdio.h>
#include "../testing.h"

#define INIT_HASHMAP(v_size, k_size, hash) hashmap m; hashmap_init(&m, v_size, k_size, hash);

uint64_t test_hash(void* v) {
	uint64_t* i = (uint64_t*)v;
	return *i;
}

uint64_t probe_hash(void* v) {
	return 0;
}

uint8_t test_hashmap_init() {
	INIT_HASHMAP(sizeof(int), sizeof(int), test_hash);

	ASSERT(m.buckets != NULL);
	ASSERT(m.hash != NULL);
	ASSERT(m.b_count != 0);
	ASSERT(m.count == 0);
	ASSERT(m.v_size == sizeof(int));
	ASSERT(m.k_size == sizeof(int));

	hashmap_deinit(&m);
	return 1;
}

uint8_t test_hashmap_deinit() {
	INIT_HASHMAP(sizeof(int), sizeof(int), test_hash);

	hashmap_deinit(&m);
	ASSERT(m.buckets == NULL);
	return 1;
}

uint8_t test_hashmap_put() {
	INIT_HASHMAP(sizeof(int), sizeof(uint64_t), test_hash);

	int v = 5;
	uint64_t k = 0;

	hashmap_put(&m, &k, &v);

	int* rv = m.buckets[0].value;	
	ASSERT(rv != NULL);
	ASSERT(*rv == 5);

	hashmap_deinit(&m);
	return 1;
}

uint8_t test_hashmap_put_ext() {
	INIT_HASHMAP(sizeof(int), sizeof(uint64_t), test_hash);

	int v = 8;
	uint64_t k = UINT64_MAX;

	hashmap_put(&m, &k, &v);

	int* rv = m.buckets[k % m.b_count].value;	
	ASSERT(rv != NULL);
	ASSERT(*rv == v);

	hashmap_deinit(&m);
	return 1;
}

uint8_t test_hashmap_put_probe() {
	INIT_HASHMAP(sizeof(int), sizeof(int), probe_hash);

	for (int i = 0; i < 10; i++) {
		hashmap_put(&m, &i, &i);
	}

	int* values = hashmap_values(&m);
	for (int i = 0; i < m.count; i++) {
		printf("v: %d\n", values[i]);
	}

	return 1;
}

uint8_t test_hashmap() {
	INIT_TEST();

	EXECUTE_SUBTEST(test_hashmap_init);
	EXECUTE_SUBTEST(test_hashmap_deinit);
	EXECUTE_SUBTEST(test_hashmap_put);
	EXECUTE_SUBTEST(test_hashmap_put_ext);
	EXECUTE_SUBTEST(test_hashmap_put_probe);

	EXIT_TEST();
}
