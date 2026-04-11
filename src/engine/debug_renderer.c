#include "debug_renderer.h"
#include "rendering/passes/debug_pass.h"
#include <stdint.h>

// void debug_set_color(vec3 color);

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


