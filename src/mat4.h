#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"

typedef struct mat4 {
	float data[16];
} mat4;

void mat4_identity(mat4* m);

void mat4_mul_vec3(const mat4 m, vec3* v);

void mat4_scale(mat4* m, const float s);

void mat4_translate(mat4* m, const vec3 v);

void mat4_rotate_x(mat4* m, const float a);
void mat4_rotate_y(mat4* m, const float a);
void mat4_rotate_z(mat4* m, const float a);

void mat4_rotate(mat4* m, const vec3 v);

void mat4_print(mat4 m);

#endif
