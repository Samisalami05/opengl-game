#include "transform.h"
#include "math/vec3.h"

void transform_init(void* v) {
	transform* t = v;
	t->position = (vec3){0};
	t->rotation = (vec3){0};
	t->scale	= (vec3){0};
}

void transform_deinit(void* v) {
	
}

void transform_update(void* v, float deltatime) {

}

void transform_render(void* v) {

}

void transform_render_debug(void* v) {

}
