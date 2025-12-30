#include "test_shader_parsing.h"
#include "../testing.h"
#include "../../src/shader.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t test_shader_parse_no_include() {
	size_t size;
	char* source = shader_parse_new("shaders/basic.vert", &size);
	printf("%s", source);
	
	TEST_ASSERT(source != NULL);
	free(source);
	return 1;
}

uint8_t test_shader_parsing() {
	INIT_TEST();

	EXECUTE_SUBTEST(test_shader_parse_no_include);

	EXIT_TEST();
}
