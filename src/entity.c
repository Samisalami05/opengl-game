#include "entity.h"

void entity_init(entity* e, mesh* m) {
	e->position = (vec3){0, 0, 0};
	e->rotation = (vec3){0, 0, 0};
	e->mesh = m;
}
