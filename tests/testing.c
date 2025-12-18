#include "testing.h"
#include <stdio.h>


void execute_test(testfunc test) {
	printf("executing test\n");
	test();
}
