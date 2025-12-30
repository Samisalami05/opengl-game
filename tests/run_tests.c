#include "arraylist/test_arraylist.h"
#include "hashmap/test_hashmap.h"
#include "shader_parsing/test_shader_parsing.h"
#include <stdio.h>
#include "testing.h"

int main(void) {
	printf("\n===== STARTED TESTING =====\n");
	test_init(&g_test_env);
	EXECUTE_TEST(test_arraylist);
	EXECUTE_TEST(test_hashmap);
	EXECUTE_TEST(test_shader_parsing);
	if (g_test_env.failed == 0) 
		printf("\n%s===== TESTS PASSED =====%s\n", COL_GREEN, COL_ENDC);
	else 
		printf("\n%s===== TESTS FAILED =====%s\n", COL_FAIL, COL_ENDC);
	printf("time: %ld ms , passed %d/%d\n\n", test_time(g_test_env), g_test_env.total - g_test_env.failed, g_test_env.total);
}
