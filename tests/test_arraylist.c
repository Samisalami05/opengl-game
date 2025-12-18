#include "test_arraylist.h"
#include <stdio.h>
#include "../src/arraylist.h"
#include "testing.h"

uint8_t test_arraylist_init();
uint8_t test_arraylist_append();

uint8_t test_arraylist_init() {
	arraylist a;
	arraylist_init(&a, 100);
	ASSERT(1);
	return 1;
}

uint8_t test_arraylist_append() {
	return 1;
}

uint8_t test_arraylist() {
	test_arraylist_init();
}
