#include "hash.h"

size_t str_hash(const void* v) { // TODO: Make this faster
	const char* str = v;
	size_t hash = 0;
	int id = 0;
	while (*str) {
		hash += (id*256)+(*(int*)str);
		str++;
	}

	return hash;
}
