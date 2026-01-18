#include "vec2.h"
#include <math.h>
#include <stdio.h>

float vec2_magnitude(const vec2 v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}

vec2 vec2_normalized(const vec2 v) {
	float mag = vec2_magnitude(v);
	if (mag <= 0) return v;

	return (vec2){
		.x = v.x / mag,
		.y = v.y / mag
	};
}

vec2 vec2_add_v(const vec2 v1, const vec2 v2) {
	return (vec2){
		.x = v1.x + v2.x,
		.y = v1.y + v2.y
	};
}

vec2 vec2_mul_f(const vec2 v1, const float v2) {
	return (vec2) {
		.x = v1.x * v2,
		.y = v1.y * v2
	};
}

void vec2_print(const vec2 v) {
	printf("%f, %f\n", v.x, v.y);
}
