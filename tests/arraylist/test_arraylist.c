#include "test_arraylist.h"
#include <stdio.h>
#include "../../src/arraylist.h"
#include "../testing.h"

#define INIT_ARRAYLIST(stride) arraylist a; arraylist_init(&a, stride);

uint8_t test_arraylist_init();
uint8_t test_arraylist_append();

uint8_t test_arraylist_init() {
	INIT_ARRAYLIST(sizeof(int));

	ASSERT(a.allocated == 0);
	ASSERT(a.count == 0);
	ASSERT(a.data == NULL);
	ASSERT(a.stride == sizeof(int));

	return 1;
}

uint8_t test_arraylist_append() {
	INIT_ARRAYLIST(sizeof(int));

	int i = 5;
	arraylist_append(&a, &i);

	int j = a.data[0];
	ASSERT(i == j);
	return 1;
}

uint8_t test_arraylist_append_ext() {
	INIT_ARRAYLIST(sizeof(long));


	int count = 100;
	long data[count];
	for (int i = 0; i < count; i++) {
		data[i] = i;
		arraylist_append(&a, data + i);
	}

	for (int i = 0; i < count; i++) {
		ASSERT(*(long*)a.data + i == i);
	}
	return 1;
}

uint8_t test_arraylist() {
	INIT_TEST();

	EXECUTE_SUBTEST(test_arraylist_init);
	EXECUTE_SUBTEST(test_arraylist_append);
	EXECUTE_SUBTEST(test_arraylist_append_ext);

	EXIT_TEST();
}
