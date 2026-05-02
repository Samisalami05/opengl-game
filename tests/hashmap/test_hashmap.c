#include "test_hashmap.h"
#include <util/hashmap.h>
#include <stdint.h>
#include <stdio.h>
#include "../testing.h"
#include <stdlib.h>

// Simple int hash
size_t int_hash(const void* key) {
    return (*(const int*)key) * 2654435761u;
}

// Helper
#define expect(condition, msg) \
	do { \
    if (!(condition)) { \
        fprintf(stderr, "FAIL %s:%d - %s\n", __func__, __LINE__, msg); \
		hashmap_deinit(&map); \
        return false; \
	} \
    } while (0); \

bool test_init_deinit() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), int_hash);
    hashmap_deinit(&map);
	return true;
}

bool test_insert_get() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), int_hash);

    int key = 42;
    int value = 100;

    expect(hashmap_put(&map, &key, &value), "insert failed");

    int* result = hashmap_get(&map, &key);
    expect(result != NULL, "get returned NULL");
    expect(*result == 100, "value mismatch");

    hashmap_deinit(&map);
	return true;
}

bool test_get_missing() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), int_hash);

    int key = 1;
    expect(hashmap_get(&map, &key) == NULL, "expected NULL for missing key");

    hashmap_deinit(&map);
	return true;
}

bool test_overwrite() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), int_hash);

    int key = 5;
    int v1 = 10;
    int v2 = 20;

    hashmap_put(&map, &key, &v1);
    hashmap_put(&map, &key, &v2);

    int* result = hashmap_get(&map, &key);
    expect(result && *result == 20, "overwrite failed");

    hashmap_deinit(&map);
	return true;
}

bool test_remove() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), int_hash);

    int key = 7;
    int value = 77;

    hashmap_put(&map, &key, &value);

    expect(hashmap_remove(&map, &key), "remove failed");
    expect(hashmap_get(&map, &key) == NULL, "key still present after remove");

    hashmap_deinit(&map);
	return true;
}

bool test_remove_missing() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), int_hash);

    int key = 123;
    expect(!hashmap_remove(&map, &key), "expected remove to fail");

    hashmap_deinit(&map);
	return true;
}

bool test_clear() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), int_hash);

    for (int i = 0; i < 100; i++) {
        int k = i, v = i * 10;
        hashmap_put(&map, &k, &v);
    }

    hashmap_clear(&map);

    for (int i = 0; i < 100; i++) {
        int k = i;
        expect(hashmap_get(&map, &k) == NULL, "clear failed");
    }

    hashmap_deinit(&map);
	return true;
}

size_t bad_hash(const void* key) {
    (void)key;
    return 1; // everything collides
}

bool test_collisions() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), bad_hash);

    for (int i = 0; i < 50; i++) {
        int k = i, v = i + 100;
        hashmap_put(&map, &k, &v);
    }

    for (int i = 0; i < 50; i++) {
        int k = i;
        int* v = hashmap_get(&map, &k);
        expect(v && *v == i + 100, "collision retrieval failed");
    }

    hashmap_deinit(&map);
	return true;
}

bool test_stress() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), int_hash);

    const int N = 10000;

    for (int i = 0; i < N; i++) {
        int k = i, v = i * 2;
        expect(hashmap_put(&map, &k, &v), "insert failed");
    }

    for (int i = 0; i < N; i++) {
        int k = i;
        int* v = hashmap_get(&map, &k);
        expect(v && *v == i * 2, "stress get failed");
    }

    hashmap_deinit(&map);
	return true;
}

bool test_stack_key_safety() {
    Hashmap map;
    hashmap_init(&map, sizeof(int), sizeof(int), int_hash);

    int value = 55;

    {
        int key = 99;
        hashmap_put(&map, &key, &value);
    }

    int lookup = 99;
    int* v = hashmap_get(&map, &lookup);

    expect(v && *v == 55, "key not copied internally");

    hashmap_deinit(&map);
	return true;
}

uint8_t test_hashmap() {
	INIT_TEST();

	EXECUTE_SUBTEST(test_init_deinit);
    EXECUTE_SUBTEST(test_insert_get);
    EXECUTE_SUBTEST(test_get_missing);
    EXECUTE_SUBTEST(test_overwrite);
    EXECUTE_SUBTEST(test_remove);
    EXECUTE_SUBTEST(test_remove_missing);
    EXECUTE_SUBTEST(test_clear);
    EXECUTE_SUBTEST(test_collisions);
    EXECUTE_SUBTEST(test_stress);
    EXECUTE_SUBTEST(test_stack_key_safety);

	EXIT_TEST();
}
