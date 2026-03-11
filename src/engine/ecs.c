#include "ecs.h"
#include "entity.h"
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
	slotmap_init(&ecs->components, sizeof(hashmap));
}

void ecs_deinit(ecs* ecs) {
	slotmap_deinit(&ecs->components);
}

static uint64_t component_hash(const void* v) {
	return *(uint64_t*)v;
}

// Returns entity id
entity2 create_entity(ecs* ecs) {
	hashmap comps; // Stores components by id
	hashmap_init(&comps, sizeof(component), sizeof(entity2), component_hash);

	uint64_t id = slotmap_add(&ecs->components, &comps);
	return id;
}

void destroy_entity(ecs* ecs, entity2 e) {
	// TODO: optimization is to clear the hashmap
	hashmap* comps = &((hashmap*)ecs->components.data)[e];
	hashmap_deinit(comps);
	slotmap_remove(&ecs->components, e);
}

// The returned pointer becomes unusable after components on entity is changed
void* add_component(ecs* ecs, entity2 e, comp_id id) {
	if (id >= _types.count) {
		fprintf(stderr, "Invalid component id %ld: The component type does not exist\n", id);
		return NULL;
	}

	hashmap* comps = &((hashmap*)ecs->components.data)[e];
	component_type* c_type = arraylist_get(&_types, id);

	component c = {
		.data = malloc(c_type->size),
		.id = id,
	};

	c_type->init(c.data);

	hashmap_put(comps, &id, &c);
	return c.data;
}

void* get_component(ecs* ecs, entity2 e, comp_id id) {
	if (id >= _types.count) {
		fprintf(stderr, "Invalid component id %ld: The component type does not exist\n", id);
		return NULL;
	}

	hashmap* comps = &((hashmap*)ecs->components.data)[e];
	
	component* c = hashmap_get(comps, &id);
	return c->data;
}
