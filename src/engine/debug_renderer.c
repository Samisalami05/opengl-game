#include "debug_renderer.h"
#include "math/vec3.h"
#include "rendering/passes/debug_pass.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

void debug_set_color(vec3 color) {
	debug_change_color((vec4){color.x, color.y, color.z, 1.0f});
}

void debug_render_line(vec3 p1, vec3 p2) {
	uint32_t base = debug_pass_base();
	debug_pass_add_vertices((vec3[]){p1, p2}, 2);
	debug_pass_add_indices((uint32_t[]){base + 0, base + 1}, 2);
}

void debug_render_cube(vec3 center, vec3 rotation, vec3 size) {
	uint32_t base = debug_pass_base();

	float hx = size.x * 0.5f;
	float hy = size.y * 0.5f;
	float hz = size.z * 0.5f;

	vec3 c = center;

	vec3 vertices[8] = {
		{c.x - hx, c.y - hy, c.z - hz}, // 0
		{c.x + hx, c.y - hy, c.z - hz}, // 1
		{c.x + hx, c.y + hy, c.z - hz}, // 2
		{c.x - hx, c.y + hy, c.z - hz}, // 3
		{c.x - hx, c.y - hy, c.z + hz}, // 4
		{c.x + hx, c.y - hy, c.z + hz}, // 5
		{c.x + hx, c.y + hy, c.z + hz}, // 6
		{c.x - hx, c.y + hy, c.z + hz}, // 7
	};

	uint32_t indices[24] = {
		// bottom
		0,1, 1,2, 2,3, 3,0,
		// top
		4,5, 5,6, 6,7, 7,4,
		// sides
		0,4, 1,5, 2,6, 3,7
	};

	// add vertices
	debug_pass_add_vertices(vertices, 8);

	// offset indices
	for (int i = 0; i < 24; i++) {
		indices[i] += base;
	}

	debug_pass_add_indices(indices, 24);
}

uint32_t get_midpoint(vec3 a, vec3 b) {
	vec3 v = vec3_normalized(vec3_div_f(vec3_add_v3(a, b), 2.0f));
	uint32_t index = debug_pass_base();
	debug_pass_add_vertices(&v, 1);
	return index;
}

void debug_render_sphere(vec3 pos, float radius) {
	float t = (1.0f + sqrtf(5.0f)) / 2.0f;

	radius /= 2.0f;

	vec3 vertices[] = {
		{-1 * radius + pos.x,  t * radius + pos.y,  0 + pos.z},
		{ 1 * radius + pos.x,  t * radius + pos.y,  0 + pos.z},
		{-1 * radius + pos.x, -t * radius + pos.y,  0 + pos.z},
		{ 1 * radius + pos.x, -t * radius + pos.y,  0 + pos.z},

		{ 0 + pos.x, -1 * radius + pos.y,  t * radius + pos.z},
		{ 0 + pos.x,  1 * radius + pos.y,  t * radius + pos.z},
		{ 0 + pos.x, -1 * radius + pos.y, -t * radius + pos.z},
		{ 0 + pos.x,  1 * radius + pos.y, -t * radius + pos.z},

		{ t * radius + pos.x,  0 + pos.y, -1 * radius + pos.z},
		{ t * radius + pos.x,  0 + pos.y,  1 * radius + pos.z},
		{-t * radius + pos.x,  0 + pos.y, -1 * radius + pos.z},
		{-t * radius + pos.x,  0 + pos.y,  1 * radius + pos.z}
	};

	uint32_t indices[] = {
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,

		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,

		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,

		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};


	uint32_t base = debug_pass_base();

	debug_pass_add_vertices(vertices, sizeof(vertices) / sizeof(vec3));

	for (int i = 0; i < sizeof(indices) / sizeof(uint32_t); i += 3) {
		debug_pass_add_indices((uint32_t[]){
			base + indices[i], base + indices[i + 1],
			base + indices[i + 1], base + indices[i + 2],
			base + indices[i + 2], base + indices[i]
		}, 6);
	}
}
