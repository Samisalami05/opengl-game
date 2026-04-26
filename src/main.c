#include <stdint.h>
#include <stdio.h>
#include <glad_impl.h>
#include "audio/wav.h"
#include "components/transform.h"
#include "debug_renderer.h"
#include "ecs.h"
#include "editor/editor.h"
#include "engine.h"
#include "engine/engine.h"
#include "engine/modelloader.h"
#include "inputmanager.h"
#include "keys.h"
#include "logger.h"
#include "profiler.h"
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
#include <stdlib.h>
#include <string.h>

// OpenAL
#include <AL/al.h>
#include <AL/alc.h>


#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

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

    game game;
	if (engine_init(&game)) return 1;
	Engine* engine = engine_get();

	editor_init(game.window);


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

	WavFile file;
	if (wav_open(&file, "assets/wat_u_want_2.wav") != WAV_OK) {
		LOG(LOG_ERROR, "Failed to open wav file");
		return 1;
	}

	wav_print_info(&file);

	ALuint buffer;
	alGenBuffers(1, &buffer);

	ALenum format;
	if (file.format.channel_count == 1 && file.format.bits_per_sample == 8) format = AL_FORMAT_MONO8;
	else if (file.format.channel_count == 1 && file.format.bits_per_sample == 16) format = AL_FORMAT_MONO16;
	else if (file.format.channel_count == 2 && file.format.bits_per_sample == 8) format = AL_FORMAT_STEREO8;
	else if (file.format.channel_count == 2 && file.format.bits_per_sample == 16) format = AL_FORMAT_STEREO16;
	else {
		fprintf(stderr, "Unsupported wav file format\n");
		return 1;
	}

	if (format == AL_FORMAT_STEREO8 || format == AL_FORMAT_STEREO16) printf("cooked\n");

	uint8_t* data = malloc(file.data_size);
	if (wav_read(&file, data, file.data_size) != WAV_OK) {
		LOG(LOG_ERROR, "Could not read data from wav file");
		return 1;
	}

	alBufferData(buffer, format, data, file.data_size, file.format.sample_rate);

	free(data);

	ALuint source;
    alGenSources(1, &source);
    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1.0f);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_TRUE);
    alSourcei(source, AL_BUFFER, buffer);

	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);

	float orientation[] = {
		0, 0, -1,  // forward
		0, 1, 0    // up
	};
	alListenerfv(AL_ORIENTATION, orientation);

	alSourcef(source, AL_REFERENCE_DISTANCE, 1.0f);
	alSourcef(source, AL_MAX_DISTANCE, 100.0f);
	alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);

    alSourcePlay(source);

	ALint state = AL_PLAYING;
	alGetSourcei(source, AL_SOURCE_STATE, &state);

    while (!glfwWindowShouldClose(game.window))
    {
		engine_begin_frame(&game);
		editor_begin_frame();

		if (state == AL_PLAYING) {
			alListener3f(AL_POSITION,
				scene->cam.pos.x,
				scene->cam.pos.y,
				scene->cam.pos.z);


			vec3 forward = camera_forward(scene->cam);
			vec3 up = camera_up(scene->cam);
			
			float orientation[] = {
				forward.x,
				forward.y,
				forward.z,
				
				up.x,
				up.y,
				up.z
			};

			alListenerfv(AL_ORIENTATION, orientation);
			alGetSourcei(source, AL_SOURCE_STATE, &state);
		}
		else printf("stopped\n");
	
		camera_key_input(&scene->cam, game.deltatime);
		camera_mouse_input(&scene->cam, game.deltatime);

		if (isKeyDown(KEY_F)) {
			LOG(LOG_INFO, "This is an info %d", engine->renderer.context.frame_index);
		}
		if (isKeyDown(KEY_G)) {
			LOG(LOG_WARNING, "This is an warning %d", engine->renderer.context.frame_index);
		}
		if (isKeyDown(KEY_H)) {
			LOG(LOG_ERROR, "This is an error %d", engine->renderer.context.frame_index);
		}

		player->rotation.y += game.deltatime / 2;

		light* sun = arraylist_get(&scene->lights, 0);

		sun->dir.x = cosf(glfwGetTime() / 4);
		sun->dir.z = sinf(glfwGetTime() / 4);

		debug_render_cube((vec3){0}, (vec3){0}, (vec3){1, 1, 1});
		
		editor_update();

		render(&engine->renderer, &m, 1, &sm_get_current_scene()->cam);
		editor_render();

		profiler_update(game.deltatime);
		engine_end_frame(&game);
    }

	wav_close(&file);

	alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
	
	//mesh_delete(triangle);
	mesh_delete(cube);
	model_deinit(&m);
	engine_deinit(&game);
    return 0;
}
