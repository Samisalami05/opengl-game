#include "vec2.h"
#include <stdio.h>

vec2 vec2_mul_f(const vec2 v1, const float v2) {
	return (vec2) {
		.x = v1.x * v2,
		.y = v1.y * v2,
	};
}

void vec2_print(const vec2 v) {
	printf("%f, %f\n", v.x, v.y);
}
