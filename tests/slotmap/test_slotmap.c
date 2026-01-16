#include "test_slotmap.h"
#include <stdio.h>
#include "engine/util/slotmap.h"
#include "util/slotmap.h"
#include "../testing.h"

uint8_t test_slotmap_init();
uint8_t test_slotmap_deinit();
uint8_t test_slotmap_append();
uint8_t test_slotmap_append_ext();

uint8_t test_slotmap_init() {
	slotmap sm;
	slotmap_init(&sm, sizeof(int));
	
	TEST_ASSERT(sm.allocated == 0);
	TEST_ASSERT(sm.count == 0);
	TEST_ASSERT(sm.data == NULL);
	TEST_ASSERT(sm.elem_size == sizeof(int));

	slotmap_deinit(&sm);
	return 1;
}

uint8_t test_slotmap_deinit() {
	slotmap sm;
	slotmap_init(&sm, sizeof(int));

	slotmap_deinit(&sm);

	TEST_ASSERT(sm.data == NULL);

	return 1;
}

uint8_t test_slotmap_append() {
	slotmap sm;
	slotmap_init(&sm, sizeof(int));

	int i = 5;
	slotmap_add(&sm, &i);

	int j = sm.data[0];
	TEST_ASSERT(i == j);

	slotmap_deinit(&sm);
	return 1;
}

uint8_t test_slotmap_append_ext() {
	slotmap sm;
	slotmap_init(&sm, sizeof(long));

	int count = 100;
	long data[count];
	for (int i = 0; i < count; i++) {
		data[i] = i;
		slotmap_add(&sm, data + i);
	}

	slotmap_remove(&sm, 3);

	for (int i = 0; i < count; i++) {
		if (slotmap_is_occupied(&sm, i)) continue;
		printf("value");
		//TEST_ASSERT(*(long*)sm.data + i == i);
	}

	slotmap_deinit(&sm);
	return 1;
}

uint8_t test_slotmap() {
	INIT_TEST();

	EXECUTE_SUBTEST(test_slotmap_init);
	EXECUTE_SUBTEST(test_slotmap_deinit);
	EXECUTE_SUBTEST(test_slotmap_append);
	EXECUTE_SUBTEST(test_slotmap_append_ext);

	EXIT_TEST();
}
