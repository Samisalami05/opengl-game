#ifndef ECS_H
#define ECS_H

#include "entity.h"
#include "util/arraylist.h"
#include "util/slotmap.h"
#include <stddef.h>
#include <stdint.h>

typedef uint64_t entity2;
typedef uint64_t comp_id;

typedef struct {
    comp_id id;
    void* data;
} component;

typedef struct {
    size_t size;

    void (*init)(void*);
    void (*deinit)(void*);
    void (*update)(void*, float);
    void (*render)(void*);
} component_type;

typedef struct {
	slotmap components;	
} ecs;

comp_id register_component(component_type type);
void type_component(comp_id id, component_type* out);

void ecs_init(ecs* ecs);
void ecs_deinit(ecs* ecs);

entity2 create_entity(ecs* ecs);
void destroy_entity(ecs* ecs, entity2 e);

// returns component
void* add_component(ecs* ecs, entity2 e, comp_id id);
void* get_component(ecs* ecs, entity2 e, comp_id id);

#endif
