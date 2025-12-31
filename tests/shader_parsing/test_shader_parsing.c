#include "test_shader_parsing.h"
#include "../testing.h"
#include "core/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLFW/glfw3.h>

uint8_t test_shader_parse_no_include() {
	char* source = shader_parse("shaders/basic.vert");
	
	TEST_ASSERT(source != NULL);
	TEST_ASSERT(strlen(source) > 0);

	free(source);
	return 1;
}

uint8_t test_shader_parse_include() {
	char* source = shader_parse("shaders/texture_lit.frag");
	
	TEST_ASSERT(source != NULL);
	TEST_ASSERT(strlen(source) > 0);

	free(source);
	return 1;
}

uint8_t test_shader_parsing() {
	INIT_TEST();

	EXECUTE_SUBTEST(test_shader_parse_no_include);

	EXIT_TEST();
}
