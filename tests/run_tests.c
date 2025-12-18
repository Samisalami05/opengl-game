#include "test_arraylist.h"
#include <stdio.h>
#include "testing.h"

int main(void) {
	uint8_t passed = 1;
	printf("--- STARTED TESTING ---\n");
	EXECUTE_TEST(test_arraylist);
	if (passed) 
		printf("--- TESTS PASSED ---\n");
	else 
		printf("--- TESTS FAILED ---\n");
}
