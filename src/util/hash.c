#include "hash.h"

uint64_t str_hash(const void* v) { // TODO: Make this faster
	const char* str = v;
	uint64_t hash = 0;
	int id = 0;
	while (*str) {
		hash += (id*256)+(*(int*)str);
		str++;
	}

	return hash;
}
