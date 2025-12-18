#ifndef TESTING
#define TESTING

#define COL_GREEN "\033[92m"
#define COL_WARN "\033[93m"
#define COL_FAIL "\033[91m"
#define COL_ENDC "\033[0m"
#define COL_BOLD "\033[1m"
#define COL_UNDERLINE "\033[4m"

#define ASSERT(x) if (!(uint8_t)(x)) { fprintf(stderr, "%s%s:%d: Assert failed%s\n", COL_FAIL, __func__, __LINE__, COL_ENDC); return 0; }
#define EXECUTE_TEST(test) printf("\n--< %s >--\n", #test); g_test_env.total++; if (!test()) { fprintf(stderr, "--< %s%s: failed%s >--\n", COL_FAIL, #test, COL_ENDC); g_test_env.failed++; } \
	else { printf("--< %s%s: passed%s >--\n", COL_GREEN, #test, COL_ENDC); }
#define EXECUTE_SUBTEST(test) curr_test.total++; if (!test()) { fprintf(stderr, " %s\uf467%s Test %s %sfailed%s\n", COL_FAIL, COL_ENDC, #test, COL_FAIL, COL_ENDC); curr_test.failed++; } \
	else { printf(" %s\uf42e%s Test %s %spassed%s\n", COL_GREEN, COL_ENDC, #test, COL_GREEN, COL_ENDC); }

#define INIT_TEST()	testenv curr_test = {0}; \
					test_init(&curr_test);
#define EXIT_TEST() long time = test_time(curr_test); \
					printf("   %d/%d - %ld ms\n", curr_test.total - curr_test.failed, curr_test.total, time); \
					return curr_test.failed == 0;

#include <stdint.h>
#include <time.h>

typedef struct testenv {
    int total;
    int failed;
	struct timespec start;
} testenv;

static testenv g_test_env = {0};

typedef uint8_t (*testfunc)(); 

void execute_test(testfunc test);

void test_init(testenv* test);
long test_time(testenv test);

#endif
