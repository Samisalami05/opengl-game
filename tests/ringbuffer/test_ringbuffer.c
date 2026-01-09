#include "test_ringbuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include "engine/util/ringbuffer.h"
#include "util/ringbuffer.h"
#include "../testing.h"

uint8_t test_ringbuffer_init();
uint8_t test_ringbuffer_deinit();
uint8_t test_ringbuffer_append();
uint8_t test_ringbuffer_append_ext();

uint8_t test_ringbuffer_init() {
	ringbuffer rb;
	ringbuffer_init(&rb, sizeof(int));

	TEST_ASSERT(rb.allocated == 0);
	TEST_ASSERT(rb.count == 0);
	TEST_ASSERT(rb.data == NULL);
	TEST_ASSERT(rb.v_size == sizeof(int));

	ringbuffer_deinit(&rb);
	return 1;
}

uint8_t test_ringbuffer_deinit() {
	ringbuffer rb;
	ringbuffer_init(&rb, sizeof(int));
	ringbuffer_deinit(&rb);

	TEST_ASSERT(rb.data == NULL);

	return 1;
}

uint8_t test_ringbuffer_append() {
	ringbuffer rb;
	ringbuffer_init(&rb, sizeof(int));

	int i = 5;
	ringbuffer_append(&rb, &i);

	int j = ((int*)rb.data)[0];
	TEST_ASSERT(i == j);

	ringbuffer_deinit(&rb);
	return 1;
}

uint8_t test_ringbuffer_remove() {
	ringbuffer rb;
	ringbuffer_init(&rb, sizeof(long));

	int count = 100;
	long data[count];
	for (int i = 0; i < count; i++) {
		data[i] = i;
		ringbuffer_append(&rb, data + i);
	}

	for (int i = 0; i < count; i++) {
		long* v = ringbuffer_remove_copy(&rb);
		TEST_ASSERT(*v == i);
		free(v);
	}

	ringbuffer_deinit(&rb);
	return 1;
}

uint8_t test_ringbuffer_remove_raw() {
	ringbuffer rb;
	ringbuffer_init(&rb, sizeof(long));

	int count = 100;
	long data[count];
	for (int i = 0; i < count; i++) {
		data[i] = i;
		ringbuffer_append(&rb, data + i);
	}

	for (int i = 0; i < count; i++) {
		long* v = ringbuffer_remove(&rb);
		TEST_ASSERT(*v == i);
	}

	ringbuffer_deinit(&rb);
	return 1;
}

uint8_t test_ringbuffer() {
	INIT_TEST();

	EXECUTE_SUBTEST(test_ringbuffer_init);
	EXECUTE_SUBTEST(test_ringbuffer_deinit);
	EXECUTE_SUBTEST(test_ringbuffer_append);
	EXECUTE_SUBTEST(test_ringbuffer_remove);
	EXECUTE_SUBTEST(test_ringbuffer_remove_raw);

	EXIT_TEST();
}
