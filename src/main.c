#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "mat4.h"
#include "mesh.h"
#include "shader.h"
#include "vec3.h"
#include "camera.h"

#ifndef TESTING	

void error_callback(int error, const char* description);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void process_input(GLFWwindow* window, float deltatime);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

typedef struct gameinfo {
	camera cam;
	float deltatime;
} gameinfo;

int main(void) {
    if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize glfw\n");
        return 1;
	}

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

	vertex vertices[] = {
		{{-0.5, -0.5, 0}, {0, 0}, {0, 0, 0}},
		{{0.5, -0.5, 0}, {1, 0}, {0, 0, 0}},
		{{0.0, 0.5, 0}, {0.5, 1}, {0, 0, 0}},
	};

	unsigned int indices[] = {
		0, 1, 2
	};

	mesh* triangle = mesh_create(vertices, 3, indices, 3);
	mesh* cube = mesh_load_obj("assets/cube.obj");

	shader shader; 
	shader_init(&shader, "shaders/basic.vert", "shaders/basic.frag");

	shader_use(shader);
	mesh_use(triangle);
	mesh_use(cube);

	gameinfo info;
	camera_init(&info.cam, 800, 800);
	info.cam.pos.z = 2;

	glfwSetWindowUserPointer(window, &info);
	glfwSetCursorPosCallback(window, mouse_callback);

	float last_frame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

		float current_frame = glfwGetTime();
		info.deltatime = current_frame - last_frame;
		last_frame = current_frame;

		process_input(window, info.deltatime);

		mat4 model;
		mat4_identity(&model);
		mat4_translate(&model, (const vec3){0, 0, 0});
		mat4_rotate(&model, (const vec3){0, 0, 0});

		mat4 view = camera_view(&info.cam);
		mat4 projection = camera_proj(&info.cam);

		shader_set_mat4(shader, "model", model);
		shader_set_mat4(shader, "view", view);
		shader_set_mat4(shader, "projection", projection);


		glDrawElements(GL_TRIANGLES, triangle->index_count, GL_UNSIGNED_INT, 0);

		glDrawElements(GL_TRIANGLES, cube->index_count, GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	mesh_delete(triangle);
	mesh_delete(cube);
	shader_deinit(shader);

    glfwTerminate();
    return 0;
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "%d Error: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	gameinfo* info = (gameinfo*)glfwGetWindowUserPointer(window);
    glViewport(0, 0, width, height);
	info->cam.width = width;
	info->cam.height = height;
}

static void process_input(GLFWwindow* window, float deltatime) {
	gameinfo* info = (gameinfo*)glfwGetWindowUserPointer(window);
	camera* cam = &info->cam;

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
	gameinfo* info = (gameinfo*)glfwGetWindowUserPointer(window);
	
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
	xoffset *= sensitivity * info->deltatime;
	yoffset *= sensitivity * info->deltatime;

	camera* cam = &info->cam;

	cam->rot.y += xoffset;
	cam->rot.x += yoffset;
}

#endif
