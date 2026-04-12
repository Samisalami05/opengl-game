#include <stdint.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "components/transform.h"
#include "debug_renderer.h"
#include "ecs.h"
#include "engine.h"
#include "engine/engine.h"
#include "engine/modelloader.h"
#include "inputmanager.h"
#include "util/arraylist.h"
#include "entity.h"
#include "lighting/light.h"
#include "material.h"
#include "core/mesh.h"
#include "rendering/renderer.h"
#include "resourcemanager.h"
#include "scene.h"
#include "scenemanager.h"
#include "math/vec3.h"
#include "rendering/camera.h"
#include "modelloader.h"
#include <math.h>

int main(void) {
	component_type type = {
		.size = sizeof(transform),
		.init = transform_init,
		.deinit = transform_deinit,
		.update = transform_update,
		.render = transform_render,
		.render_debug = transform_render_debug,
	};

	register_component(type);

    game* game = engine_init();
	if (game == NULL) return 1;

	model m;
	load_model(&m, "assets/diner/scene.gltf");
	
	mesh* cube = mesh_load_obj_new("assets/person.obj");
	mesh* plane = mesh_load_obj_new("assets/cube.obj");

	material ground_mat, player_mat;
	material_init(&ground_mat, MAT_TEXTURE_LIT);
	material_init(&player_mat, MAT_COLOR_LIT);
	ground_mat.tiling = (vec2){{ 100.0f, 100.0f }};
	player_mat.shininess = 100.0f;
	
	entity* player = entity_create(cube, &player_mat);
	entity* ground = entity_create(plane, &ground_mat);

	player->position.y += 2.5f;
	player->scale = (vec3){{ 5, 5, 5 }};
	ground->scale.x = 100;
	ground->scale.z = 100;
	
	ground_mat.albedo_tex = load_texture("assets/grass.jpg");
	
	scene* scene = sm_get_current_scene();

	entity2 ent = create_entity(); // TODO: figure out why this code does stuff to the lighting
	add_component(ent, 0);
	transform* comp = get_component(ent, 0);

	vec3_print(comp->position);

	destroy_entity(ent);
	
	light pointlight1, pointlight2;
	light_init_point(&pointlight1, (vec3){{ 2.0f, 6.0f, 1.0f }});
	light_init_point(&pointlight2, (vec3){{ -2.0f, 16.0f, -2.0f }});
	pointlight1.color = (vec3){{ 1.0f, 0.5f, 0.2f }};
	pointlight2.color = (vec3){{ 0.5f, 0.2f, 1.0f }};
	pointlight1.intensity = 0.7f;
	pointlight2.intensity = 0.5f;
	pointlight1.range = 10.0f;

	arraylist_append(&scene->lights, &pointlight1);
	arraylist_append(&scene->lights, &pointlight2);

	int i = 0;

    while (!glfwWindowShouldClose(game->window))
    {
		engine_begin_frame(game);
		camera_key_input(&scene->cam, game->deltatime);
		camera_mouse_input(&scene->cam, game->deltatime);

		player->rotation.y += game->deltatime / 2;

		light* sun = arraylist_get(&scene->lights, 0);

		sun->dir.x = cosf(glfwGetTime() / 4);
		sun->dir.z = sinf(glfwGetTime() / 4);
		
		// FPS counter
		if (i < glfwGetTime()) {
			printf("fps: %f\n", 1 / game->deltatime);
			i+=2;
		}

		const int size = 50;

		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {
				for (int z = 0; z < size; z++) {
					debug_set_color((vec3){(float)x / size, (float)y / size, (float)z / size});
					debug_render_cube((vec3){x, y, z}, (vec3){0}, (vec3){1.0f, 1.0f, 1.0f});
				}
			}
		}

		if (isKeyDown(KEY_F)) {
			debug_set_color((vec3){0.0f, 0.0f, 1.0f});
			debug_render_line((vec3){0}, (vec3){0, 100.0f, 50.0f});
			debug_render_line((vec3){0}, (vec3){0, -50.0f, 300.0f});
		}

		// Rendering
		render(&game->renderer, &m, 1, &sm_get_current_scene()->cam);
		//render_scene(sm_get_current_scene());
		//render_mesh(cube, &m.materials[0], &sm_get_current_scene()->cam);
		//render_model(&m, &sm_get_current_scene()->cam,
		//	(vec3){0.0f, 0.5f, 0.0f},
		//	(vec3){0},
		//	(vec3){3.0f, 3.0f, 3.0f}
		//);
		
		engine_end_frame(game);
    }

	//mesh_delete(triangle);
	mesh_delete(cube);
	model_deinit(&m);
	engine_deinit(game);
    return 0;
}
