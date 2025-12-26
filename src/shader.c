#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

/* ------------------ Internal Declarations ------------------- */

static void shader_compile(unsigned int shader, char* source);
static char* shader_parse(const char* shader_path);

/* -------------------- Internal Functions -------------------- */

static void shader_compile(unsigned int shader, char* source) {
	glShaderSource(shader, 1, (const GLchar* const*)&source, NULL);
	glCompileShader(shader);
	
	int success;
	char info_log[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		fprintf(stderr, "ERROR: Shader compilation failed, shader program: %d\n%s", shader, info_log);
	}
}

static char* shader_parse(const char* shader_path) {
	FILE* fp = fopen(shader_path, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file: %s\n", shader_path);
		return NULL;
	}

	int rc = fseek(fp, 0L, SEEK_END);
	if (rc == -1) {
		perror("fseek");
		return NULL;
	}

	long size = ftell(fp);
	if (size == -1) {
		perror("ftell");
		return NULL;
	}

	rewind(fp);
	char* source = malloc(size + 1);
	
	fread(source, 1, size, fp);
	source[size] = '\0';

	fclose(fp);

	return source;
}

/* ------------------ External Declarations ------------------- */

void shader_init(shader* s, const char* vertsh, const char* fragsh) {
	char* vertex_source = shader_parse(vertsh);
	char* fragment_source = shader_parse(fragsh);

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	shader_compile(vertex, vertex_source);

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	shader_compile(fragment, fragment_source);	

	free(vertex_source);
	free(fragment_source);

	s->id = glCreateProgram();

	glAttachShader(s->id, vertex);
	glAttachShader(s->id, fragment);
	glLinkProgram(s->id);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void shader_use(shader s) {
	glUseProgram(s.id);
}

void shader_deinit(shader s) {
	glDeleteProgram(s.id);
}

void shader_set_int(shader s, const char* n, int v) {
	glUniform1i(glGetUniformLocation(s.id, n), v);
}
void shader_set_uint(shader s, const char* n, unsigned int v) {
	glUniform1ui(glGetUniformLocation(s.id, n), v);
}

void shader_set_float(shader s, const char* n, float v) {
	glUniform1f(glGetUniformLocation(s.id, n), v);
}

void shader_set_vec2(shader s, const char* n, vec2 v) {
	glUniform2f(glGetUniformLocation(s.id, n), v.x, v.y);
}

void shader_set_vec3(shader s, const char* n, vec3 v) {
	glUniform3f(glGetUniformLocation(s.id, n), v.x, v.y, v.z);
}

void shader_set_vec4(shader s, const char* n, vec4 v) {
	glUniform4f(glGetUniformLocation(s.id, n), v.x, v.y, v.z, v.w);
}

void shader_set_mat4(shader s, const char* n, mat4 v) {
	glUniformMatrix4fv(glGetUniformLocation(s.id, n), 1, GL_FALSE, (const float*)v.data);
}
