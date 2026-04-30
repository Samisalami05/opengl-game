#include "map.h"
#include "allocator.h"
#include "assimp/types.h"
#include "logger.h"
#include "math/vec2.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define CURR_BUCKET map->capacity
#define TMP_BUCKET (map->capacity + 1)
#define OCCUPIED_MASK (1 << ((sizeof(uint32_t) * 8) - 1))

static uint32_t bucket_size(Hashmap* map) {
	return map->k_size + map->v_size + sizeof(uint32_t);
}

static uint32_t bucket_key_offset(Hashmap* map, size_t id) {
	return bucket_size(map) * id;
}

static uint32_t bucket_value_offset(Hashmap* map, size_t id) {
	return bucket_key_offset(map, id) + map->k_size;
}

static uint32_t bucket_probe_offset(Hashmap* map, size_t id) {
	return bucket_value_offset(map, id) + map->v_size;
}

static void* bucket_key(Hashmap* map, size_t id) {
	return map->buckets + bucket_key_offset(map, id);
}

static void* bucket_value(Hashmap* map, size_t id) {
	return map->buckets + bucket_value_offset(map, id);
}

static uint32_t* bucket_probe(Hashmap* map, size_t id) {
	return (uint32_t*)(map->buckets + bucket_probe_offset(map, id));
}

static void bucket_cpy(Hashmap* map, size_t dest, size_t src) {
	memcpy(map->buckets + bucket_key_offset(map, dest), map->buckets + bucket_key_offset(map, src), bucket_size(map));
}

static void bucket_set(Hashmap* map, size_t id, void* key, void* value, uint32_t probe, bool occupied) {
	memcpy(bucket_key(map, id), key, map->k_size);
	memcpy(bucket_value(map, id), value, map->v_size);

	uint32_t new_probe = probe;
	if (occupied) new_probe |= OCCUPIED_MASK;
	memcpy(bucket_probe(map, id), &new_probe, sizeof(uint32_t));
}

static bool bucket_is_occupied(Hashmap* map, size_t id) {
	return (*bucket_probe(map, id) & OCCUPIED_MASK);
}

void map_init(Hashmap* map, size_t k_size, size_t v_size, HashFunc func) {
	map->k_size = k_size;
	map->v_size = v_size;
	map->hash = func;
}

static void bucket_swap(Hashmap* map, uint32_t a, uint32_t b) {
	// TODO: i think its "capacity - 1"
	void* tmp = bucket_key(map, TMP_BUCKET);
	memcpy(tmp, bucket_key(map, a), bucket_size(map));
	memcpy(bucket_key(map, a), bucket_key(map, b), bucket_size(map));
	memcpy(bucket_key(map, b), tmp, bucket_size(map));
}

static bool map_expand(Hashmap* map) {
	printf("\n --- Resizing --- \n");
    size_t old_capacity = map->capacity;
    size_t new_capacity = old_capacity == 0 ? 4 : old_capacity * 2;

    void* old_buckets = map->buckets;

    void* new_buckets = CALLOC(new_capacity + 2, bucket_size(map)); // +2 for CURR and TMP bucket
    if (!new_buckets) {
        LOG(LOG_ERROR, "calloc failed: %s", strerror(errno));
        return false;
    }

    map->buckets = new_buckets;
    map->capacity = new_capacity;
    map->count = 0;

    // Reinsert old entries
    for (size_t i = 0; i < old_capacity; i++) {
        uint8_t* old = (uint8_t*)old_buckets;

		uint32_t* probe = (uint32_t*)(old + bucket_probe_offset(map, i));
		if (!(*probe & OCCUPIED_MASK)) continue;
		printf("Inserting\n");
		
		void* key = old + bucket_key_offset(map, i);
		void* value = old + bucket_value_offset(map, i);

		if (!map_insert(map, key, value)) {
			FREE(new_buckets);
			map->buckets = old_buckets;
			map->capacity = old_capacity;
			return false;
		}
    }

    FREE(old_buckets);
	printf(" --- Done resizing --- \n\n");
    return true;
}


bool map_insert(Hashmap* map, void* key, void* value) {
	if (map->count + 1 > map->capacity)
		if (!map_expand(map)) return false;

	bucket_set(map, CURR_BUCKET, key, value, 0, true);

	uint64_t p = map->hash(key) % map->capacity;
	
	printf("Adding key %d with hash %ld\n", *(int*)key, p);

	uint32_t vpsl = 0;  // probe sequence length
	while (bucket_is_occupied(map, p)) {
		printf("bucket at %ld is not null\n", p);
		if (vpsl >= map->capacity) {
			if (map_expand(map)) {
				return false;
			}
			p = map->hash(key) % map->capacity;
			vpsl = 0;
			continue;
		}

		if (memcmp(bucket_key(map, p), bucket_key(map, CURR_BUCKET), map->k_size) == 0) {
			printf("keys are equal\n");
			break;
		}

		if (vpsl > (*bucket_probe(map, p) & ~OCCUPIED_MASK)) {
			*bucket_probe(map, p) = (vpsl | OCCUPIED_MASK);
			bucket_swap(map, p, CURR_BUCKET);
		}

		p = (p + 1) % map->capacity;
		vpsl++;
	}

	bucket_cpy(map, p, CURR_BUCKET);
	*bucket_probe(map, p) = (vpsl | OCCUPIED_MASK);
	map->count++;

	for (int i = 0; i < map->capacity; i++) {
		if (!bucket_is_occupied(map, i)) continue;
		printf("[{%.1f, %.1f}, %d, %d] ", (*(vec2*)bucket_value(map, i)).x, (*(vec2*)bucket_value(map, i)).y, *(int*)bucket_key(map, i), *(uint32_t*)bucket_probe(map, i) & ~OCCUPIED_MASK);
	}
	printf("\n");
	return true;
}
