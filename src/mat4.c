#include "mat4.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static int index_to_1d(int x, int y) {
	return y * 4 + x;
}

void mat4_identity(mat4* m) {
	memset(m->data, 0, sizeof(m->data));
	for (int i = 0; i < 4; i++) {
		int index = index_to_1d(i, i);
		m->data[index] = 1;
	}
}

void mat4_mul(mat4* m1, const mat4 m2);

void mat4_mul_vec3(const mat4 m, vec3* v) {
	v->x = m.data[0] * v->x + m.data[3];
	v->y = m.data[4] * v->y + m.data[7];
	v->z = m.data[8] * v->z + m.data[11];
}

void mat4_scale(mat4* m, const float s) {
	for (int i = 0; i < 4; i++) {
		int index = index_to_1d(i, i);
		m->data[index] *= s;	
	}
}

void mat4_translate(mat4* m, const vec3 v) {
	m->data[12] = v.x;
	m->data[13] = v.y;
	m->data[14] = v.z;
}

void mat4_rotate_x(mat4* m, const float a) {
	mat4_identity(m);
	m->data[5] = cos(a);
	m->data[6] = -sin(a);
	m->data[9] = sin(a);
	m->data[10] = cos(a);
}

void mat4_rotate_y(mat4* m, const float a) {
	mat4_identity(m);
	m->data[0] = cos(a);
	m->data[2] = sin(a);
	m->data[8] = -sin(a);
	m->data[10] = cos(a);
}

void mat4_rotate_z(mat4* m, const float a) {
	mat4_identity(m);
	m->data[0] = cos(a);
	m->data[1] = -sin(a);
	m->data[4] = sin(a);
	m->data[5] = cos(a);
}

void mat4_rotate(mat4* m, const vec3 v) {
	mat4 rx, ry, rz;
	mat4_rotate_x(&rx, v.x);
	mat4_rotate_y(&ry, v.y);
	mat4_rotate_z(&rz, v.z);

	mat4_mul(m, rx);
	mat4_mul(m, ry);
	mat4_mul(m, rz);
}

void mat4_print(mat4 m) {
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			int index = index_to_1d(x, y);
			printf("%f ", m.data[index]);
		}
		printf("\n");
	}
}
