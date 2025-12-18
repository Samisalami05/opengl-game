#ifndef TESTING
#define TESTING

#define ASSERT(x) if (!x) { fprintf(stderr, "%s:%d: Assert failed\n", __func__, __LINE__); return 0; }
#define EXECUTE_TEST(test) if (!test()) { fprintf(stderr, "Test %s failed\n", #test); passed = 0; } \
	else { printf("Test %s passed\n", #test); }

#include <stdint.h>

typedef uint8_t (*testfunc)(); 

void execute_test(testfunc test);

#endif
