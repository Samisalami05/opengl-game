#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <stddef.h>
#include <stdint.h>
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"
#include <raylib.h>

typedef struct shader {
	unsigned int id;
} shader;

char* shader_parse(const char* shader_path); // For testing

uint8_t shader_init(shader* s, const char* vertsh, const char* fragsh);
void shader_use(const shader* s);
void shader_deinit(shader* s);

// Uniforms
void shader_set_int(const shader* s, const char* n, int v);
void shader_set_uint(const shader* s, const char* n, unsigned int v);
void shader_set_float(const shader* s, const char* n, float v);
void shader_set_vec2(const shader* s, const char* n, vec2 v);
void shader_set_vec3(const shader* s, const char* n, vec3 v);
void shader_set_vec4(const shader* s, const char* n, vec4 v);
void shader_set_mat4(const shader* s, const char* n, mat4 v);

#endif
