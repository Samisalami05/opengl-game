#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include "math/vec3.h"

void debug_set_color(vec3 color);
void debug_render_line(vec3 p1, vec3 p2);
void debug_render_cube(vec3 pos, vec3 rotation, vec3 scale);
void debug_render_sphere(vec3 pos, float radius, int res);

/*
 * TODO: 
 * debug_render_box()
 * debug_render_aabb()
 * debug_render_frustum()
 * debug_render_sphere()
 */

#endif
