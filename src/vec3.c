#include "vec3.h"
#include <math.h>

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
