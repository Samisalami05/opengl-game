
#ifndef HASHMAP_STR_H
#define HASHMAP_STR_H

#include <stddef.h>
#include <stdint.h>

typedef uint64_t (*str_hashfunc)(const void*);

typedef struct {
	void* value;
	char* key;
	uint32_t probe; // probe sequence length
} bucket_str;

typedef struct {
	bucket_str* buckets;
	size_t b_count;
	size_t count;
	size_t v_size;
	str_hashfunc hash;
} hashmap_str;

void hashmap_str_init(hashmap_str* m, size_t v_size, str_hashfunc hash);
void hashmap_str_init_detailed(hashmap_str* m, size_t v_size, size_t buckets, str_hashfunc hash);
void hashmap_str_deinit(hashmap_str* m);

void* hashmap_str_put(hashmap_str* m, const char* k, const void* v);
void* hashmap_str_get(hashmap_str* m, const char* k);
void* hashmap_str_values(hashmap_str* m);
char** hashmap_str_keys(hashmap_str* m);

#endif
