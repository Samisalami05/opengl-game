#include "hashmap.h"
#include "allocator.h"
#include "logger.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define CURR_BUCKET map->capacity
#define TMP_BUCKET (map->capacity + 1)
#define OCCUPIED_MASK ((probe_t)1 << ((sizeof(probe_t) * 8) - 1))

typedef size_t probe_t;

/*
 * The hashmap stores all bucket in one buffer and is accessed with
 * bucket helper functions. This allows the hashmap to be fast with
 * the least amount of allocations. 
 *
 * The buffer is allocated to capacity + 2 in order to have two
 * temporary buckets used for swap and insert operations. Each
 * bucket also uses the last bit in the probe as a occupied bit
 * for maximum memory efficiency.
 *
 * Bucket structure
 *
 *	void* key;      // key bytes
 *	void* value;    // value bytes
 *	probe_t probe;  // probe (last bit is used for occupied)
*/

static uint32_t bucket_size(Hashmap* map);
static size_t bucket_key_offset(Hashmap* map, size_t id);
static size_t bucket_value_offset(Hashmap* map, size_t id);
static size_t bucket_probe_offset(Hashmap* map, size_t id);
static void* bucket_key(Hashmap* map, size_t id);
static void* bucket_value(Hashmap* map, size_t id);
static probe_t* bucket_probe(Hashmap* map, size_t id);
static void bucket_cpy(Hashmap* map, size_t dest, size_t src);
static void bucket_set(Hashmap* map, size_t id, const void* key, const void* value, probe_t probe, bool occupied, bool key_str);
static bool bucket_is_occupied(Hashmap* map, size_t id);
static void bucket_swap(Hashmap* map, size_t a, size_t b);
static bool bucket_cmp(Hashmap* map, size_t a, size_t b, bool key_str);
static bool hashmap_expand(Hashmap* map);

static bool hashmap_put_internal(Hashmap* map, const void* key, const void* value, bool key_str);
static void* hashmap_get_internal(Hashmap* map, const void* key, bool key_str);
static bool hashmap_remove_internal(Hashmap* map, const void* key, bool key_str);

void hashmap_init(Hashmap* map, size_t k_size, size_t v_size, HashFunc func) {
	map->k_size = k_size;
	map->v_size = v_size;
	map->hash = func;
	map->capacity = 0;
	map->count = 0;
	map->buckets = NULL;
}

void hashmap_deinit(Hashmap* map) {
	if (map->buckets != NULL) {
		FREE(map->buckets);
		map->buckets = NULL;
	}
	map->count = 0;
	map->capacity = 0;
}

bool hashmap_put(Hashmap* map, const void* key, const void* value) {
	return hashmap_put_internal(map, key, value, false);
}

// The returned pointer is invalid after insert
void* hashmap_get(Hashmap* map, const void* key) {
	return hashmap_get_internal(map, key, false);
}

bool hashmap_remove(Hashmap* map, const void* key) {
	return hashmap_remove_internal(map, key, false);
}

// Use this if your key is a string
bool hashmap_put_str(Hashmap* map, const char* key, const void* value) {
	return hashmap_put_internal(map, key, value, true);
}

void* hashmap_get_str(Hashmap* map, const char* key) {
	return hashmap_get_internal(map, key, true);
}

bool hashmap_remove_str(Hashmap* map, const char* key) {
	return hashmap_remove_internal(map, key, true);
}

void hashmap_clear(Hashmap *map) {
	// Set all buckets to non occupied
	for (size_t i = 0; i < map->capacity; i++) {
		*bucket_probe(map, i) = 0;
	}
	map->count = 0;
}

void hashmap_keys(Hashmap* map, void* out) {
	for (size_t i = 0; i < map->capacity; i++) {
		if (!bucket_is_occupied(map, i)) continue;
		memcpy(out + i * map->k_size, bucket_key(map, i), map->k_size);
	}
}

void hashmap_values(Hashmap* map, void* out) {
	for (size_t i = 0; i < map->capacity; i++) {
		printf("i: %ld\n", i);
		if (!bucket_is_occupied(map, i)) continue;
		memcpy((uint8_t*)out + i * map->v_size, bucket_value(map, i), map->v_size);
	}
}

static void bucket_set(Hashmap* map, size_t id, const void* key, const void* value, probe_t probe, bool occupied, bool key_str) {
	if (key != NULL) {
		if (!key_str) memcpy(bucket_key(map, id), key, map->k_size);
		else strncpy(bucket_key(map, id), key, map->k_size);
	}
	if (value != NULL) memcpy(bucket_value(map, id), value, map->v_size);

	probe_t new_probe = probe;
	if (occupied) new_probe |= OCCUPIED_MASK;
	memcpy(bucket_probe(map, id), &new_probe, sizeof(probe_t));
}

static bool bucket_is_occupied(Hashmap* map, size_t id) {
	return (*bucket_probe(map, id) & OCCUPIED_MASK);
}

static void bucket_swap(Hashmap* map, size_t a, size_t b) {
	// TODO: i think its "capacity - 1"
	void* tmp = bucket_key(map, TMP_BUCKET);
	memcpy(tmp, bucket_key(map, a), bucket_size(map));
	memcpy(bucket_key(map, a), bucket_key(map, b), bucket_size(map));
	memcpy(bucket_key(map, b), tmp, bucket_size(map));
}

static bool bucket_cmp(Hashmap* map, size_t a, size_t b, bool key_str) {
	if (key_str && strncmp(bucket_key(map, a), bucket_key(map, b), map->k_size) == 0) return true;
	else if (!key_str && memcmp(bucket_key(map, a), bucket_key(map, b), map->k_size) == 0) return true;

	return false;
}

static bool hashmap_expand(Hashmap* map) {
    size_t old_capacity = map->capacity;
    size_t new_capacity = old_capacity == 0 ? 4 : old_capacity * 2;

    uint8_t* old = map->buckets;

    map->capacity = new_capacity;
    map->count = 0;

    uint8_t* new_buckets = calloc(new_capacity + 2, bucket_size(map)); // +2 for CURR and TMP bucket
    if (!new_buckets) {
        LOG(LOG_ERROR, "calloc failed: %s", strerror(errno));
        return false;
    }


    map->buckets = new_buckets;

    // Reinsert old entries
    for (size_t i = 0; i < old_capacity; i++) {
		probe_t* probe = (probe_t*)(old + bucket_probe_offset(map, i));
		if (!(*probe & OCCUPIED_MASK)) continue;
		
		void* key = old + bucket_key_offset(map, i);
		void* value = old + bucket_value_offset(map, i);

		if (!hashmap_put(map, key, value)) {
			free(new_buckets);
			map->buckets = old;
			map->capacity = old_capacity;
			return false;
		}
    }

    free(old);
    return true;
}

static bool hashmap_put_internal(Hashmap* map, const void* key, const void* value, bool key_str) {
	if (key == NULL || value == NULL) {
		LOG(LOG_ERROR, "Cant insert with a NULL %s\n", key == NULL ? "key" : "value");
		return false;
	}

	// TODO: return inserted value
	// TODO: remove probe from buckets (easy to compute)

	if (map->count + 1 > map->capacity) {
		printf("Expanding: %ld\n", map->capacity);
		if (!hashmap_expand(map)) return false;
		printf("Done expanding: %ld\n", map->capacity);
	}

	bucket_set(map, CURR_BUCKET, key, value, 0, true, key_str);

	size_t p = map->hash(key) % map->capacity;
	
	probe_t probe = 0;  // probe sequence length
	printf("p: %ld, hash: %ld\n", p, map->hash(key));
	while (bucket_is_occupied(map, p)) {
		if (probe >= map->capacity) {
			if (hashmap_expand(map)) {
				return false;
			}
			p = map->hash(key) % map->capacity;
			probe = 0;
			continue;
		}

		if (bucket_cmp(map, p, CURR_BUCKET, key_str)) {
			break;
		}

		if (probe > (*bucket_probe(map, p) & ~OCCUPIED_MASK)) {
			*bucket_probe(map, p) = (probe | OCCUPIED_MASK);
			bucket_swap(map, p, CURR_BUCKET);
		}

		p = (p + 1) % map->capacity;
		probe++;
	}

	bucket_cpy(map, p, CURR_BUCKET);
	*bucket_probe(map, p) = (probe | OCCUPIED_MASK);
	map->count++;

	return true;
}

static void* hashmap_get_internal(Hashmap* map, const void* key, bool key_str) {
	if (key == NULL || map->capacity == 0) return NULL;

	size_t steps = 0;
	size_t p = map->hash(key) % map->capacity;
	for (;;) {
		if (steps >= map->capacity) {
			return NULL;
		}

		if (bucket_is_occupied(map, p)) {
			if (key_str) { if (strncmp(bucket_key(map, p), key, map->k_size) == 0) break;}
			else if (memcmp(bucket_key(map, p), key, map->k_size) == 0) break;
		}

		p = (p + 1) % map->capacity;
		steps++;
	}
	return bucket_value(map, p);
}

static bool hashmap_remove_internal(Hashmap* map, const void* key, bool key_str) {
	if (key == NULL || map->capacity == 0) return false;

	size_t p = map->hash(key) % map->capacity;
	size_t probe = 0;

	for (;;) {
		if (!bucket_is_occupied(map, p)) return false;

		probe_t curr_probe = (*(probe_t*)bucket_probe(map, p) & ~OCCUPIED_MASK);

		if (probe > curr_probe) return false;
		
		if (bucket_is_occupied(map, p)) {
			if (key_str) { if (strncmp(bucket_key(map, p), key, map->k_size) == 0) break;}
			else if (memcmp(bucket_key(map, p), key, map->k_size) == 0) break;
		}

		p = (p + 1) % map->capacity;
		probe++;
	}

	size_t hole = p;
	size_t next = (p + 1) % map->capacity;

	while (bucket_is_occupied(map, next) && (*(probe_t*)bucket_probe(map, next) & ~OCCUPIED_MASK) > 0) {
		bucket_cpy(map, hole, next);
		probe_t* b_probe = bucket_probe(map, hole);
		*b_probe = ((*b_probe & ~OCCUPIED_MASK) - 1);

		hole = next;
		next = (next + 1) % map->capacity;
	}

	bucket_set(map, hole, NULL, NULL, 0, false, false);
	map->count--;

	return true;
}

static uint32_t bucket_size(Hashmap* map) {
	return map->k_size + map->v_size + sizeof(probe_t);
}

static size_t bucket_key_offset(Hashmap* map, size_t id) {
	return bucket_size(map) * id;
}

static size_t bucket_value_offset(Hashmap* map, size_t id) {
	return bucket_key_offset(map, id) + map->k_size;
}

static size_t bucket_probe_offset(Hashmap* map, size_t id) {
	return bucket_value_offset(map, id) + map->v_size;
}

static void* bucket_key(Hashmap* map, size_t id) {
	return map->buckets + bucket_key_offset(map, id);
}

static void* bucket_value(Hashmap* map, size_t id) {
	return map->buckets + bucket_value_offset(map, id);
}

static probe_t* bucket_probe(Hashmap* map, size_t id) {
	return (probe_t*)(map->buckets + bucket_probe_offset(map, id));
}

static void bucket_cpy(Hashmap* map, size_t dest, size_t src) {
	memcpy(map->buckets + bucket_key_offset(map, dest), map->buckets + bucket_key_offset(map, src), bucket_size(map));
}
