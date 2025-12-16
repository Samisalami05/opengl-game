#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "mat4.h"
#include "mesh.h"
#include "shader.h"
#include "vec3.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

	mesh* mesh = mesh_create(vertices, 3, indices, 3);

	shader shader; 
	shader_init(&shader, "shaders/basic.vert", "shaders/basic.frag");

	shader_use(shader);
	mesh_use(mesh);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

		mat4 model;
		mat4_identity(&model);
		mat4_translate(&model, (const vec3){0, 0, 0});

		mat4 view;
		mat4_identity(&view);
		mat4_rotate(&view, (const vec3){1, 2, 1});
		//camera_get_view(main_camera, view);

		mat4 projection;
		mat4_identity(&projection);
		//camera_get_projection(main_camera, projection);

		shader_set_mat4(shader, "model", model);
		shader_set_mat4(shader, "view", view);
		shader_set_mat4(shader, "projection", projection);


		glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	mesh_delete(mesh);
	shader_deinit(shader);

    glfwTerminate();
    return 0;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

