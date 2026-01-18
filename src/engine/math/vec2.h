#ifndef VEC2_H
#define VEC2_H

typedef struct vec2 {
	float x;
	float y;
} vec2;

vec2 vec2_mul_f(const vec2 v1, const float v2);

void vec2_print(const vec2 v);

#endif
