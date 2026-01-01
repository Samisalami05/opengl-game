#ifndef DEBUG_RENDERER_H
#define DEBUG_RENDERER_H

#include "vec3.h"

void debug_set_color(vec3 color);
void debug_render_line(vec3 p1, vec3 p2);
void debug_render_cube(vec3 pos, vec3 rotation, vec3 scale);
void debug_render_cube_wireframe(vec3 pos, vec3 rotation, vec3 scale);

#endif
