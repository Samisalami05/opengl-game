#include "light.h"

void light_init_global(light* light, const vec3 dir) {
	light->type = LIGHT_GLOBAL;
	light->intensity = 1.0f;
	light->color = (vec3){1.0f, 1.0f, 1.0f};
	light->dir = dir;
}
void light_init_point(light* light, const vec3 pos) {
	light->type = LIGHT_POINT;
	light->intensity = 1.0f;
	light->range = 5.0f;
	light->color = (vec3){1.0f, 1.0f, 1.0f};
	light->position = pos;
}
