#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------ Internal Declarations ------------------- */

static int line_offset(char* source, int line);
static void print_line(char* source, int line);
static int line_tabs(char* source, int line);
static void print_location_begin(int line);
static void print_error_location(char* source, int line, int column);
static int str_skip_chars_off(const char* str, char c, int count);
static void log_message(char* message, const char* log, int length);
static void shader_compile_error(const char* path, unsigned int shader, char* source);
static uint8_t shader_compile(const char* name, unsigned int shader, char* source);
static char* expand_str(char* str, size_t new_size);
static char* shader_parse(const char* shader_path);

/* -------------------- Internal Functions -------------------- */

static int line_offset(char* source, int line) {
	if (line <= 0) fprintf(stderr, "shader: Invalid line number %d\n", line);
	if (line == 1) return 0;

	int offset = 0;
	int line_num = 1;
	while (*source != '\0') {
		offset++;
		if (*source == '\n') {
			line_num++;
			if (line_num == line) {
				return offset; // Might be +1
			}
		}
		source++;
	}
	return -1;
}

static void print_line(char* source, int line) {
	int offset = line_offset(source, line);
	if (offset == -1) {
		fprintf(stderr, "shader: Could not print line %d: Does not exist in shader\n", line);
		return;
	}
	source += offset;

	while (*source != '\n' && *source != '\0') {
		printf("%c", *source);
		source++;
	}
	printf("\n");
}

static int line_tabs(char* source, int line) {
	int offset = line_offset(source, line);
	if (offset == -1) {
		fprintf(stderr, "shader: Could not print line %d: Does not exist in shader\n", line);
		return 0;
	}
	source += offset;

	int tabs = 0;
	while (*source == '\t') {
		tabs++;
		source++;
	}
	return tabs;
}

static void print_location_begin(int line) {
	if (line > 0) 
		fprintf(stderr, "%5d | ", line);
	else 
		fprintf(stderr, "      | ");
}

static void print_error_location(char* source, int line, int column) {
	print_location_begin(line);
	print_line(source, line);

	print_location_begin(-1);
	int tabs = line_tabs(source, line);
	for (int i = 0; i < tabs; i++) {
		printf("\t");
	}

	for (int i = tabs - 1; i < column; i++) {
		printf(" ");
	}
	printf("^\n");
}

static int str_skip_chars_off(const char* str, char c, int count) {
	int offset = 0;
	while (count > 0 && str[offset] != EOF) {
		if (str[offset] == c) count--;
		offset++;
	}

	return offset;
}

static void log_message(char* message, const char* log, int length) {
	int offset = str_skip_chars_off(log, ':', 2);
	strncpy(message, log + offset + 1, length); // +1 to remove space after colon
}

static void shader_compile_error(const char* path, unsigned int shader, char* source) {
	int info_length;
	char info_log[512];
	glGetShaderInfoLog(shader, 512, &info_length, info_log);

	char message[info_length - 8 + 1]; // 8 is the minimum size of the beginning of info log
	log_message(message, info_log, sizeof(message));

	int tmp; // TODO: what does this do
	int line_num;
	int column;
	sscanf(info_log, "%d:%d(%d): ", &tmp, &line_num, &column);
	fprintf(stderr, "%s:%d:%d: %s", path, line_num, column, message);
	print_error_location(source, line_num, column);
}

static uint8_t shader_compile(const char* path, unsigned int shader, char* source) {
	glShaderSource(shader, 1, (const GLchar* const*)&source, NULL);
	glCompileShader(shader);
	
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		shader_compile_error(path, shader, source);
		return 1;
	}
	return 0;
}

static char* expand_str(char* str, size_t new_size) {
	if (str == NULL) {
		str = malloc(new_size + 1);
		if (str == NULL) perror("shader: malloc");
		str[0] = '\0';
	}
	else {
		char* tmp = realloc(str, new_size + 1);
		if (tmp == NULL) perror("shader: realloc");
		str = tmp;
	}
	return str;
}

char* shader_parse_new(const char* shader_path, size_t* size_out) {
	FILE* fp = fopen(shader_path, "rb");
	if (fp == NULL) {
		fprintf(stderr, "shader parser: Cant open file %s: No such file or directory\n", shader_path);
		perror("shader parser");
		return NULL;
	}

	size_t size = 0;
	char* content = NULL;

	char line[1024];
	while (fgets(line, sizeof(line), fp) != NULL) {
		size_t line_size = strlen(line);
		if (strncmp(line, "#include ", 9) == 0) {
			char include_path[256];
            sscanf(line, "#include \"%255[^\"]\"", include_path);

			size_t inc_size;
			char* include_content = shader_parse_new(include_path, &inc_size);
			if (include_content == NULL) {
				free(content);
				return NULL;
			}

			size += inc_size;
			content = realloc(content, size + 1); // +1 for \0
			strcat(content, include_content);
			free(include_content);
		}
		else {
			size += line_size;
			content = expand_str(content, size + 1); // p1 for \0
			strcat(content, line);
		}
	}

	fclose(fp);
	*size_out = size;
	return content;
}

/* ------------------ External Declarations ------------------- */

void shader_init(shader* s, const char* vertsh, const char* fragsh) {
	size_t s1, s2;
	char* vertex_source = shader_parse_new(vertsh, &s1);
	char* fragment_source = shader_parse_new(fragsh, &s2);

	if (vertex_source == NULL || fragment_source == NULL) return;

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	shader_compile(vertsh, vertex, vertex_source);

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	shader_compile(fragsh, fragment, fragment_source);

	free(vertex_source);
	free(fragment_source);

	s->id = glCreateProgram();

	glAttachShader(s->id, vertex);
	glAttachShader(s->id, fragment);
	glLinkProgram(s->id);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void shader_use(shader* s) {
	glUseProgram(s->id);
}

void shader_deinit(shader* s) {
	glDeleteProgram(s->id);
}

void shader_set_int(shader* s, const char* n, int v) {
	glUniform1i(glGetUniformLocation(s->id, n), v);
}
void shader_set_uint(shader* s, const char* n, unsigned int v) {
	glUniform1ui(glGetUniformLocation(s->id, n), v);
}

void shader_set_float(shader* s, const char* n, float v) {
	glUniform1f(glGetUniformLocation(s->id, n), v);
}

void shader_set_vec2(shader* s, const char* n, vec2 v) {
	glUniform2f(glGetUniformLocation(s->id, n), v.x, v.y);
}

void shader_set_vec3(shader* s, const char* n, vec3 v) {
	glUniform3f(glGetUniformLocation(s->id, n), v.x, v.y, v.z);
}

void shader_set_vec4(shader* s, const char* n, vec4 v) {
	glUniform4f(glGetUniformLocation(s->id, n), v.x, v.y, v.z, v.w);
}

void shader_set_mat4(shader* s, const char* n, mat4 v) {
	glUniformMatrix4fv(glGetUniformLocation(s->id, n), 1, GL_FALSE, (const float*)v.data);
}
