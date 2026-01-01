#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <stddef.h>
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"

typedef struct shader {
	unsigned int id;
} shader;

char* shader_parse(const char* shader_path); // For testing

void shader_init(shader* s, const char* vertsh, const char* fragsh);
void shader_use(shader* s);
void shader_deinit(shader* s);

// Uniforms
void shader_set_int(shader* s, const char* n, int v);
void shader_set_uint(shader* s, const char* n, unsigned int v);
void shader_set_float(shader* s, const char* n, float v);
void shader_set_vec2(shader* s, const char* n, vec2 v);
void shader_set_vec3(shader* s, const char* n, vec3 v);
void shader_set_vec4(shader* s, const char* n, vec4 v);
void shader_set_mat4(shader* s, const char* n, mat4 v);

#endif
