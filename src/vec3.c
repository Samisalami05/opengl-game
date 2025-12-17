#include "vec3.h"
#include <math.h>
#include <stdio.h>

float vec3_magnitude(const vec3 v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

void vec3_normilize(vec3* v) {
	float mag = vec3_magnitude(*v);

	v->x /= mag;
	v->y /= mag;
	v->z /= mag;
}

vec3 vec3_normilized(const vec3 v) {
	float mag = vec3_magnitude(v);

	return (vec3){
		.x = v.x / mag,
		.y = v.y / mag,
		.z = v.z / mag,
	};
}

float vec3_dot(const vec3 v1, const vec3 v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

vec3 vec3_inverse(const vec3 v) {
	return (vec3){-v.x, -v.y, -v.z};
}

vec3 vec3_cross(const vec3 v1, const vec3 v2) {
	return (vec3){
		v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x
	};
}

void vec3_add_v3(vec3* v1, const vec3 v2) {
	v1->x += v2.x;
	v1->y += v2.y;
	v1->z += v2.z;
}

void vec3_sub_v3(vec3* v1, const vec3 v2) {
	v1->x -= v2.x;
	v1->y -= v2.y;
	v1->z -= v2.z;
}

void vec3_mul_v3(vec3* v1, const vec3 v2) {
	v1->x *= v2.x;
	v1->y *= v2.y;
	v1->z *= v2.z;
}

void vec3_div_v3(vec3* v1, const vec3 v2) {
	v1->x /= v2.x;
	v1->y /= v2.y;
	v1->z /= v2.z;
}

void vec3_add_f(vec3* v1, const float v2) {
	v1->x += v2;
	v1->y += v2;
	v1->z += v2;
}

void vec3_sub_f(vec3* v1, const float v2) {
	v1->x -= v2;
	v1->y -= v2;
	v1->z -= v2;
}

void vec3_mul_f(vec3* v1, const float v2) {
	v1->x *= v2;
	v1->y *= v2;
	v1->z *= v2;
}

void vec3_div_f(vec3* v1, const float v2) {
	v1->x /= v2;
	v1->y /= v2;
	v1->z /= v2;
}

void vec3_print(const vec3 v) {
	printf("%f, %f, %f\n", v.x, v.y, v.z);
}
