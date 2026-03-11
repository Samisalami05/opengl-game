#include "arraylist.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static uint8_t expand_if_necessary(arraylist* a) {
	if (a->allocated <= a->count) {
		a->allocated = a->allocated <= 0 ? 4 : a->allocated * 2;
		void* tmp = realloc(a->data, a->stride * a->allocated);
		if (tmp == NULL) {
			perror("arraylist: realloc");
			return 1;
		}
		a->data = tmp;
	}
	return 0;
}

void arraylist_init(arraylist* a, size_t stride) {
	a->data = NULL;
	a->allocated = 0;
	a->count = 0;
	a->stride = stride;
}

void arraylist_deinit(arraylist* a) {
	if (a->data != NULL)
		free(a->data);
	a->data = NULL;
}

uint8_t arraylist_append(arraylist* a, void* data) {
	if (expand_if_necessary(a)) return 0;
	memcpy(a->data + a->count * a->stride, data, a->stride);
	a->count++;
	return 1;
}

uint8_t arraylist_set(arraylist* a, void* data, uint64_t index) {
	if (index < 0 || index >= a->count) {
		fprintf(stderr, "arraylist: Index %ld out of bounds in list with count %d\n", index, a->count);
		return 0;
	}
	memcpy(a->data + index * a->stride, data, a->stride);
	return 1;
}

void* arraylist_get(arraylist* a, uint64_t index) {
	if (index < 0 || index >= a->count) {
		fprintf(stderr, "arraylist: Index %ld out of bounds in list with count %d\n", index, a->count);
		return NULL;
	}
	return a->data + index * a->stride;
}

void* arraylist_get_last(arraylist* a) {
	if (a->count == 0) {
		fprintf(stderr, "arraylist: Cant get last element from a empty lis\nt");
		return NULL;
	}
	return a->data + (a->count - 1) * a->stride;
}

uint8_t arraylist_remove(arraylist* a, uint64_t index) {
	if (a->count == 0) {
		fprintf(stderr, "arraylist: Cant remove last element from a empty list\n");
		return 0;
	}

	if (index < a->count - 1)
		memcpy(a->data + index * a->stride, a->data + (index + 1) * a->stride, (a->count - index - 1) * a->stride);

	a->count--;
	return 1;
}

uint8_t arraylist_remove_last(arraylist* a) {
	if (a->count == 0) {
		fprintf(stderr, "arraylist: Cant remove last element from a empty list\n");
		return 0;
	}
	a->count--;
	return 1;
}


