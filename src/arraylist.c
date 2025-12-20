#include "arraylist.h"
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

void arraylist_append(arraylist* a, void* data) {
	if (expand_if_necessary(a)) return;
	memcpy(a->data + a->count * a->stride, data, a->stride);
	a->count++;
}

void arraylist_deinit(arraylist* a) {
	if (a->data != NULL)
		free(a->data);
	a->data = NULL;
}
