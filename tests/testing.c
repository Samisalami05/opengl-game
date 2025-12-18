#include "testing.h"
#include <stdio.h>

// Not used
void execute_test(testfunc test) {
	printf("executing test\n");
	test();
}

void test_init(testenv* test) {
	test->total = 0;
	test->failed = 0;
	clock_gettime(CLOCK_MONOTONIC, &test->start);
}

long test_time(testenv test) {
	struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    time_t sec = now.tv_sec - test.start.tv_sec;
    long nsec  = now.tv_nsec - test.start.tv_nsec;

    if (nsec < 0) {
        sec--;
        nsec += 1000000000L;
    }

    return sec * 1000L + nsec / 1000000L;
}
