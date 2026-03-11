#include "vec3.h"
#include <math.h>
#include <stdio.h>

float vec3_magnitude(const vec3 v) {
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

void vec3_normalize(vec3* v) {
	float mag = vec3_magnitude(*v);
	if (mag <= 0) return;

	v->x /= mag;
	v->y /= mag;
	v->z /= mag;
}

vec3 vec3_normalized(const vec3 v) {
	float mag = vec3_magnitude(v);
	if (mag <= 0) return v;

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

vec3 vec3_add_v3(const vec3 v1, const vec3 v2) {
	return (vec3) {
		.x = v1.x + v2.x,
		.y = v1.y + v2.y,
		.z = v1.z + v2.z
	};
}

vec3 vec3_sub_v3(const vec3 v1, const vec3 v2) {
	return (vec3) {
		.x = v1.x - v2.x,
		.y = v1.y - v2.y,
		.z = v1.z - v2.z
	};
}

vec3 vec3_mul_v3(const vec3 v1, const vec3 v2) {
	return (vec3) {
		.x = v1.x * v2.x,
		.y = v1.y * v2.y,
		.z = v1.z * v2.z
	};
}

vec3 vec3_div_v3(const vec3 v1, const vec3 v2) {
	return (vec3) { //TODO: handle zero division
		.x = v1.x / v2.x,
		.y = v1.y / v2.y,
		.z = v1.z / v2.z
	};
}

vec3 vec3_add_f(const vec3 v1, const float v2) {
	return (vec3) {
		.x = v1.x + v2,
		.y = v1.y + v2,
		.z = v1.z + v2
	};
}

vec3 vec3_sub_f(const vec3 v1, const float v2) {
	return (vec3) {
		.x = v1.x - v2,
		.y = v1.y - v2,
		.z = v1.z - v2
	};
}

vec3 vec3_mul_f(const vec3 v1, const float v2) {
	return (vec3) {
		.x = v1.x * v2,
		.y = v1.y * v2,
		.z = v1.z * v2
	};
}

vec3 vec3_div_f(const vec3 v1, const float v2) {
	return (vec3) {
		.x = v1.x / v2,
		.y = v1.y / v2,
		.z = v1.z / v2
	};
}

void vec3_print(const vec3 v) {
	printf("%f, %f, %f\n", v.x, v.y, v.z);
}
