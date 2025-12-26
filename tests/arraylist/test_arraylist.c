#include "test_arraylist.h"
#include <stdio.h>
#include "../../src/arraylist.h"
#include "../testing.h"

#define INIT_ARRAYLIST(stride) arraylist a; arraylist_init(&a, stride);

uint8_t test_arraylist_init();
uint8_t test_arraylist_deinit();
uint8_t test_arraylist_append();
uint8_t test_arraylist_append_ext();

uint8_t test_arraylist_init() {
	INIT_ARRAYLIST(sizeof(int));

	TEST_ASSERT(a.allocated == 0);
	TEST_ASSERT(a.count == 0);
	TEST_ASSERT(a.data == NULL);
	TEST_ASSERT(a.stride == sizeof(int));

	arraylist_deinit(&a);
	return 1;
}

uint8_t test_arraylist_deinit() {
	INIT_ARRAYLIST(sizeof(int));
	arraylist_deinit(&a);

	TEST_ASSERT(a.data == NULL);

	return 1;
}

uint8_t test_arraylist_append() {
	INIT_ARRAYLIST(sizeof(int));

	int i = 5;
	arraylist_append(&a, &i);

	int j = a.data[0];
	TEST_ASSERT(i == j);

	arraylist_deinit(&a);
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
		TEST_ASSERT(*(long*)a.data + i == i);
	}

	arraylist_deinit(&a);
	return 1;
}

uint8_t test_arraylist() {
	INIT_TEST();

	EXECUTE_SUBTEST(test_arraylist_init);
	EXECUTE_SUBTEST(test_arraylist_deinit);
	EXECUTE_SUBTEST(test_arraylist_append);
	EXECUTE_SUBTEST(test_arraylist_append_ext);

	EXIT_TEST();
}
