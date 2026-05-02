#include "ecs.h"
#include "allocator.h"
#include "entity.h"
#include "scenemanager.h"
#include "util/arraylist.h"
#include "util/hashmap.h"
#include "util/slotmap.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static arraylist _types = { NULL, 0, sizeof(component_type), 0 };

comp_id register_component(component_type type) {
	arraylist_append(&_types, &type);
	return _types.count - 1;
}

void type_component(comp_id id, component_type* out) {
	*out = ((component_type*)_types.data)[id];
}

void ecs_init(ecs* ecs) {
	slotmap_init(&ecs->components, sizeof(Hashmap));
}

void ecs_deinit(ecs* ecs) { // TODO: free all components
	slotmap_deinit(&ecs->components);
}

static uint64_t component_hash(const void* v) {
	return *(uint64_t*)v;
}

// Returns entity id
entity2 create_entity() {
	scene* s = sm_get_current_scene();
	if (s == NULL) return INVALID_ENTITY;

	Hashmap comps; // Stores components by id
	hashmap_init(&comps, sizeof(entity2), sizeof(component), component_hash);

	uint64_t id = slotmap_add(&s->ecs.components, &comps);
	return id;
}

void destroy_entity(entity2 e) {
	scene* s = sm_get_current_scene();
	if (s == NULL) return;

	// TODO: optimization is to clear the hashmap
	Hashmap* compmap = &((Hashmap*)s->ecs.components.data)[e];
	
	component comps[compmap->count];
	hashmap_values(compmap, comps);
	for (int i = 0; i < compmap->count; i++) {
		component_type type = ((component_type*)_types.data)[comps[i].id];
		if (type.deinit)
			type.deinit(comps->data);

		FREE(comps[i].data);
	}
	
	hashmap_deinit(compmap);
	slotmap_remove(&s->ecs.components, e);
}

// TODO: what happens if you add more than of the same component type.
// The returned pointer becomes unusable after components on entity is changed
void* add_component(entity2 e, comp_id id) {
	if (id >= _types.count) {
		fprintf(stderr, "Invalid component id %ld: The component type does not exist\n", id);
		return NULL;
	}

	scene* s = sm_get_current_scene();
	if (s == NULL) return NULL;

	Hashmap* comps = &((Hashmap*)s->ecs.components.data)[e];
	component_type* c_type = arraylist_get(&_types, id);

	component c = {
		.data = MALLOC(c_type->size),
		.id = id,
	};

	c_type->init(c.data);

	hashmap_put(comps, &id, &c);
	return c.data;
}

void* get_component(entity2 e, comp_id id) {
	if (id >= _types.count) {
		fprintf(stderr, "Invalid component id %ld: The component type does not exist\n", id);
		return NULL;
	}

	scene* s = sm_get_current_scene();
	if (s == NULL) return NULL;

	Hashmap* comps = &((Hashmap*)s->ecs.components.data)[e];
	
	component* c = hashmap_get(comps, &id);
	if (c == NULL) return NULL;
	return c->data;
}
