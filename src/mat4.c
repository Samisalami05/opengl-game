#include "mat4.h"
#include "vec3.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static int index_to_1d(int x, int y) {
	return x * 4 + y; // instead if y * 4 + x because opengl is column-major
}

void mat4_identity(mat4* m) {
	memset(m->data, 0, sizeof(m->data));
	for (int i = 0; i < 4; i++) {
		int index = index_to_1d(i, i);
		m->data[index] = 1;
	}
}

mat4 mat4_lookat(vec3 eye, vec3 target) {
	mat4 m;
	mat4_identity(&m);

	vec3_sub_v3(&eye, target);
	vec3 forward = vec3_normilized(eye);
	vec3 right = vec3_cross(forward, (vec3){0.0f, 1.0f, 0.0f});
	vec3 up = vec3_cross(right, forward);
	
	// Column 0
    m.data[index_to_1d(0,0)] = right.x;
    m.data[index_to_1d(0,1)] = right.y;
    m.data[index_to_1d(0,2)] = right.z;
    m.data[index_to_1d(0,3)] = 0.0f;

    // Column 1
    m.data[index_to_1d(1,0)] = up.x;
    m.data[index_to_1d(1,1)] = up.y;
    m.data[index_to_1d(1,2)] = up.z;
    m.data[index_to_1d(1,3)] = 0.0f;

    // Column 2
    m.data[index_to_1d(2,0)] = -forward.x;
    m.data[index_to_1d(2,1)] = -forward.y;
    m.data[index_to_1d(2,2)] = -forward.z;
    m.data[index_to_1d(2,3)] = 0.0f;

    // Column 3 (translation)
    m.data[index_to_1d(3,0)] = -vec3_dot(right, eye);
    m.data[index_to_1d(3,1)] = -vec3_dot(up, eye);
    m.data[index_to_1d(3,2)] = vec3_dot(forward, eye);
    m.data[index_to_1d(3,3)] = 1.0f;

	return m;
}

void mat4_mul(mat4* m1, const mat4 m2) {
	mat4 result;
	mat4_identity(&result);

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {			
			float v = 0;
			for (int i = 0; i < 4; i++) {
				int a_index = index_to_1d(i, y);
				int b_index = index_to_1d(x, i);				
				v += m1->data[a_index] * m2.data[b_index];
			}
			result.data[index_to_1d(x, y)] = v;
		}
	}

	*m1 = result;
}

void mat4_mul_vec3(const mat4 m, vec3* v) {
    float x = v->x;
    float y = v->y;
    float z = v->z;

    v->x = m.data[index_to_1d(0,0)] * x +
           m.data[index_to_1d(1,0)] * y +
           m.data[index_to_1d(2,0)] * z +
           m.data[index_to_1d(3,0)];

    v->y = m.data[index_to_1d(0,1)] * x +
           m.data[index_to_1d(1,1)] * y +
           m.data[index_to_1d(2,1)] * z +
           m.data[index_to_1d(3,1)];

    v->z = m.data[index_to_1d(0,2)] * x +
           m.data[index_to_1d(1,2)] * y +
           m.data[index_to_1d(2,2)] * z +
           m.data[index_to_1d(3,2)];
}


void mat4_scale(mat4* m, const float s) {
	for (int i = 0; i < 4; i++) {
		int index = index_to_1d(i, i);
		m->data[index] *= s;	
	}
}

void mat4_translate(mat4* m, const vec3 v) {
    m->data[index_to_1d(3,0)] +=
        m->data[index_to_1d(0,0)] * v.x +
        m->data[index_to_1d(1,0)] * v.y +
        m->data[index_to_1d(2,0)] * v.z;

    m->data[index_to_1d(3,1)] +=
        m->data[index_to_1d(0,1)] * v.x +
        m->data[index_to_1d(1,1)] * v.y +
        m->data[index_to_1d(2,1)] * v.z;

    m->data[index_to_1d(3,2)] +=
        m->data[index_to_1d(0,2)] * v.x +
        m->data[index_to_1d(1,2)] * v.y +
        m->data[index_to_1d(2,2)] * v.z;
}

void mat4_rotate_x(mat4* m, const float a) {
	mat4_identity(m);
	m->data[index_to_1d(1,1)] =  cos(a);
	m->data[index_to_1d(2,1)] = -sin(a);
	m->data[index_to_1d(1,2)] =  sin(a);
	m->data[index_to_1d(2,2)] =  cos(a);
}

void mat4_rotate_y(mat4* m, const float a) {
	mat4_identity(m);
	m->data[index_to_1d(0, 0)] = cos(a);
	m->data[index_to_1d(2, 0)] = -sin(a);
	m->data[index_to_1d(0, 2)] = sin(a);
	m->data[index_to_1d(2, 2)] = cos(a);
}

void mat4_rotate_z(mat4* m, const float a) {
	mat4_identity(m);
	m->data[index_to_1d(0, 0)] = cos(a);
	m->data[index_to_1d(1, 0)] = -sin(a);
	m->data[index_to_1d(0, 1)] = sin(a);
	m->data[index_to_1d(1, 1)] = cos(a);
}

void mat4_rotate(mat4* m, const vec3 v) {
	mat4 rx, ry, rz;
	mat4_rotate_z(&rz, v.z); // Maybe like this
	mat4_rotate_y(&ry, v.y);
	mat4_rotate_x(&rx, v.x);

	mat4_mul(m, rz);
	mat4_mul(m, ry);
	mat4_mul(m, rx);}

void mat4_projection(mat4* m, float fov, float width, float height, float near_clip, float far_clip) {
	float aspect = width / height;
	float wow = tanf(fov * 0.5f * (M_PI / 180.0f));
	float t = wow * near_clip;
	float b = -t;
	float r = t * aspect;
	float l = -r;

	m->data[0]  = 2 * near_clip / (r - l);
    m->data[5]  = 2 * near_clip / (t - b);
    m->data[8]  = (r + l) / (r - l);
    m->data[9]  = (t + b) / (t - b);
    m->data[10] = -(far_clip + near_clip) / (far_clip - near_clip);
    m->data[14] = -2 * far_clip * near_clip / (far_clip - near_clip);
    m->data[11] = -1;
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
