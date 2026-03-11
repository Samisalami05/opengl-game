#ifndef ECS_H
#define ECS_H

#include "util/slotmap.h"
#include <stddef.h>
#include <stdint.h>

#define INVALID_ENTITY UINT64_MAX

/**
 * Is an id describing an entity.
 * @note Invalid outside the scene that it was created 
 * in.
 */
typedef uint64_t entity2;

/**
 * Id indicating which type the component has.
 * @note Only points to a component type and not to a
 * component instance.
 */
typedef uint64_t comp_id;

/**
 * Is a instance of a component type.
 */
typedef struct {
    comp_id id;
    void* data;
} component;

/**
 * Describes a component and contains component behavoiur.
 */
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

/**
 * Registers a component type.
 *
 * @note Component types are seperated from entity component
 * systems.
 * @return returns the component id.
 */
comp_id register_component(component_type type);

/**
 * Get the component type from a component id.
 */
void type_component(comp_id id, component_type* out);

/**
 * Initializes the given entity component system.
 */
void ecs_init(ecs* ecs);

/**
 * Deinitializes the given entity component system.
 */
void ecs_deinit(ecs* ecs);

/**
 * Creates an entity in the entity component system.
 *
 * @note The function uses the currently loaded scene.
 * @return On success returns the entity, else returns NULL.
 */
entity2 create_entity();

/**
 * Destroys an entity in the entity component system.
 *
 * @note The function uses the currently loaded scene.
 * @param e The entity to destroy.
 *
 */
void destroy_entity(entity2 e);

/**
 * Adds a component to a entity by using a component id.
 *
 * @note The function uses the currently loaded scene.
 * @param e The entity to add to.
 * @param id The component id.
 * @return On success returns the component data, else returns
 * NULL.
 */
void* add_component(entity2 e, comp_id id);

/**
 * Get a component in a entity by using a component id.
 *
 * @note The function uses the currently loaded scene.
 * @param e The entity to get from.
 * @param id The component id.
 * @return On success returns the component data, else returns
 * NULL.
 */
void* get_component(entity2 e, comp_id id);

#endif
