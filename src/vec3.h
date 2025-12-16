#ifndef VEC3_H
#define VEC3_H

typedef struct vec3 {
	float x;
	float y;
	float z;
} vec3;

float vec3_magnitude(const vec3 v);

void vec3_normilize(vec3* v);
vec3 vec3_normilized(const vec3 v);

#endif
