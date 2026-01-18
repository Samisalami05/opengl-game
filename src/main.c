#include <math.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "core/cubemap.h"
#include "engine.h"
#include "engine/modelloader.h"
#include "inputmanager.h"
#include "keys.h"
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
#include "math/mathutil.h"
#include "core/cubemap.h"
#include "modelloader.h"
#include "util/util.h"
#include <math.h>

static float deltatime;

int main(void) {
    game* game = engine_init();

	model m;
	load_model(&m, "assets/diner/scene.gltf");
	
	mesh* cube = mesh_load_obj_new("assets/person.obj");
	mesh* plane = mesh_load_obj_new("assets/cube.obj");

	material ground_mat, player_mat;
	material_init(&ground_mat, MAT_TEXTURE_LIT);
	material_init(&player_mat, MAT_COLOR_LIT);
	ground_mat.tiling = (vec2){100.0f, 100.0f};
	player_mat.shininess = 1.0f;
	
	entity* player = entity_create(cube, &player_mat);
	entity* ground = entity_create(plane, &ground_mat);

	player->position.y += 10;
	player->scale = (vec3){5, 5, 5};
	ground->scale.x = 100;
	ground->scale.z = 100;
	
	ground_mat.albedo_tex = load_texture("assets/grass.jpg");
	
	scene* scene = sm_get_current_scene();
	
	light pointlight1, pointlight2;
	light_init_point(&pointlight1, (vec3){2.0f, 6.0f, 1.0f});
	light_init_point(&pointlight2, (vec3){-2.0f, 16.0f, -2.0f});
	pointlight1.color = (vec3){1.0f, 0.5f, 0.2f};
	pointlight2.color = (vec3){0.5f, 0.2f, 1.0f};
	pointlight1.intensity = 0.7f;
	pointlight2.intensity = 0.5f;
	pointlight1.range = 10.0f;

	arraylist_append(&scene->lights, &pointlight1);
	arraylist_append(&scene->lights, &pointlight2);

	float last_frame = 0.0f;
	
	float time = 0;
	int i = 0;

    while (!glfwWindowShouldClose(game->window))
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float current_frame = glfwGetTime();
		deltatime = current_frame - last_frame;
		last_frame = current_frame;

		camera_key_input(&scene->cam, deltatime);
		camera_mouse_input(&scene->cam);

		time += deltatime;

		//player->rotation.y += deltatime / 2;

		light* sun = arraylist_get(&scene->lights, 0);

		sun->dir.x = cosf(time / 4);
		sun->dir.z = sinf(time / 4);
		
		// FPS counter
		if (i % max((int)(1 / deltatime), 1) == 0) printf("fps: %f\n", 1 / deltatime);
		i += 1;

		inputman_update(game->window);

		// Rendering
		render_scene(sm_get_current_scene());
		render_model(&m, &sm_get_current_scene()->cam,
			(vec3){0.0f, 2.5f, 0.0f},
			(vec3){0},
			(vec3){3.0f, 3.0f, 3.0f}
		);
		
		engine_end_frame(game);
    }

	//mesh_delete(triangle);
	mesh_delete(cube);
	model_deinit(&m);
	engine_deinit(game);
    return 0;
}
