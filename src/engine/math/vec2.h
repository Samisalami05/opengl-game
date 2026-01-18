#ifndef VEC2_H
#define VEC2_H

typedef struct vec2 {
	float x;
	float y;
} vec2;

float vec2_magnitude(const vec2 v);
vec2 vec2_normalized(const vec2 v);

vec2 vec2_add_v(const vec2 v1, const vec2 v2);
vec2 vec2_mul_f(const vec2 v1, const float v2);

void vec2_print(const vec2 v);

#endif
