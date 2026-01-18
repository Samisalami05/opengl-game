#ifndef VEC3_H
#define VEC3_H

typedef struct vec3 {
	float x;
	float y;
	float z;
} vec3;

float vec3_magnitude(const vec3 v);

void vec3_normalize(vec3* v);
vec3 vec3_normalized(const vec3 v);
float vec3_dot(const vec3 v1, const vec3 v2);
vec3 vec3_inverse(const vec3 v);
vec3 vec3_cross(const vec3 v1, const vec3 v2);

vec3 vec3_add_v3(const vec3 v1, const vec3 v2);
vec3 vec3_sub_v3(const vec3 v1, const vec3 v2);
vec3 vec3_mul_v3(const vec3 v1, const vec3 v2);
vec3 vec3_div_v3(const vec3 v1, const vec3 v2);

vec3 vec3_add_f(const vec3 v1, const float v2);
vec3 vec3_sub_f(const vec3 v1, const float v2);
vec3 vec3_mul_f(const vec3 v1, const float v2);
vec3 vec3_div_f(const vec3 v1, const float v2);

void vec3_print(const vec3 v);

#endif
