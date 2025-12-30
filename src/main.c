#include <math.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "arraylist.h"
#include "entity.h"
#include "light.h"
#include "mat4.h"
#include "material.h"
#include "mesh.h"
#include "rendering.h"
#include "scene.h"
#include "scenemanager.h"
#include "shader.h"
#include "texture.h"
#include "vec3.h"
#include "camera.h"
#include "ssbo.h"

#ifndef TESTING	

void error_callback(int error, const char* description);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void process_input(GLFWwindow* window, float deltatime);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

static float deltatime;

int main(void) {
    if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize glfw\n");
        return 1;
	}

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(640, 480, "game", NULL, NULL);
    if (!window) {
        glfwTerminate();
		fprintf(stderr, "Failed to create glfw window\n");
        return 1;
    }

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetErrorCallback(error_callback);
    glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to load glad\n");
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	sm_init();
	
	mesh* cube = mesh_load_obj_new("assets/character.obj");
	mesh* plane = mesh_load_obj_new("assets/cube.obj");

	material ground_mat, player_mat;
	material_init(&ground_mat, MAT_TEXTURE_LIT);
	material_init(&player_mat, MAT_COLOR_LIT);
	ground_mat.tiling = (vec2){100.0f, 100.0f};
	player_mat.shininess = 100.0f;
	player_mat.color = (vec3){1.0f, 0.0f, 0.0f};
	
	entity* player = entity_create(cube, &player_mat);
	entity* ground = entity_create(plane, &ground_mat);

	player->position.y += 10;
	ground->scale.x = 100;
	ground->scale.z = 100;
	
	ground_mat.albedo_tex = texture_create("assets/grass.jpg", GL_RGB);
	
	scene* scene = sm_get_current_scene();
	
	light pointlight1, pointlight2;
	light_init_point(&pointlight1, (vec3){2.0f, 8.0f, 1.0f});
	light_init_point(&pointlight2, (vec3){-2.0f, 13.0f, -2.0f});
	pointlight1.color = (vec3){1.0f, 1.0f, 0.8f};
	pointlight2.color = (vec3){1.0f, 1.0f, 0.8f};

	arraylist_append(&scene->lights, &pointlight1);
	arraylist_append(&scene->lights, &pointlight2);

	glfwSetCursorPosCallback(window, mouse_callback);

	float last_frame = 0.0f;
	
	float time = 0;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float current_frame = glfwGetTime();
		deltatime = current_frame - last_frame;
		last_frame = current_frame;

		process_input(window, deltatime);

		time += deltatime / 8;

		//e->rotation.y += deltatime / 2;

		light* sun = arraylist_get(&scene->lights, 0);

		sun->dir.x = cosf(time);
		sun->dir.z = sinf(time);
		
		render_scene(sm_get_current_scene());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	//mesh_delete(triangle);
	mesh_delete(cube);
	sm_deinit();

    glfwTerminate();
    return 0;
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "%d Error: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	scene* scene = sm_get_current_scene();
	scene->cam.width = width;
	scene->cam.height = height;
}

static void process_input(GLFWwindow* window, float deltatime) {
	scene* scene = sm_get_current_scene();
	camera* cam = &scene->cam;

	float camera_speed = 10.0f;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		vec3 move = camera_right(*cam);
		vec3_mul_f(&move, deltatime * camera_speed);
		vec3_sub_v3(&cam->pos, move);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		vec3 move = camera_right(*cam);
		vec3_mul_f(&move, deltatime * camera_speed);
		vec3_add_v3(&cam->pos, move);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		vec3 move = camera_forward(*cam);
		vec3_mul_f(&move, deltatime * camera_speed);
		vec3_add_v3(&cam->pos, move);
		//vec3_sub_v3(&cam->pos, move);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		vec3 move = camera_forward(*cam);
		vec3_mul_f(&move, deltatime * camera_speed);
		vec3_sub_v3(&cam->pos, move);
		//vec3_add_v3(&cam->pos, move);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		vec3 move = {0.0f, 1.0f, 0.0f};
		vec3_mul_f(&move, deltatime * camera_speed);
		vec3_add_v3(&cam->pos, move);
		//vec3_sub_v3(&cam->pos, move);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		vec3 move = {0.0f, 1.0f, 0.0f};
		vec3_mul_f(&move, deltatime * camera_speed);
		vec3_sub_v3(&cam->pos, move);
		//vec3_add_v3(&cam->pos, move);
	}
}

char first_mouse = 1;
float last_x = 320;
float last_y = 240;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	scene* scene = sm_get_current_scene();
	
	if (first_mouse) {
		last_x = xpos;
		last_y = ypos;
		first_mouse = 0;
	}

	float xoffset = xpos - last_x;
	float yoffset = ypos - last_y;
	last_x = xpos;
	last_y = ypos;

	float sensitivity = 0.5f;
	xoffset *= sensitivity * deltatime;
	yoffset *= sensitivity * deltatime;

	camera* cam = &scene->cam;

	cam->rot.y += xoffset;
	cam->rot.x += yoffset;
}

#endif
