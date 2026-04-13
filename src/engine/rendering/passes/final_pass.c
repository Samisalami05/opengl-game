#include "final_pass.h"
#include "core/shader.h"
#include "rendering/pipeline.h"
#include "rendering/rendertexture.h"
#include "resourcemanager.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define VERTEX_COUNT 4
#define INDEX_COUNT 6

uint32_t fp_vbo;
uint32_t fp_vao;
uint32_t fp_ebo;

typedef struct {
	vec2 pos;
	vec2 uv;
} fp_vertex;

fp_vertex vertices[] = {
	{{-1.0f, -1.0f}, {0.0f, 0.0f}},
	{{ 1.0f, -1.0f}, {1.0f, 0.0f}},
	{{-1.0f,  1.0f}, {0.0f, 1.0f}},
	{{ 1.0f,  1.0f}, {1.0f, 1.0f}}
};
uint32_t fp_indices[] = {0, 1, 2, 2, 1, 3};
shader* fp_shader = NULL;

uint8_t final_pass_init(render_pass* rp, render_targets* targets) {
	strcpy(rp->name, "final_pass");
	rp->width = 640;
	rp->height = 480;
	rp->data = NULL;

	rp->execute = final_pass_execute;
	rp->resize = final_pass_resize;
	rp->deinit = final_pass_deinit;

	glGenVertexArrays(1, &fp_vao);
	glBindVertexArray(fp_vao);

	glGenBuffers(1, &fp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, fp_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fp_vertex) * VERTEX_COUNT, vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &fp_ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fp_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_COUNT * sizeof(uint32_t), fp_indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(fp_vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(fp_vertex), (void*)offsetof(fp_vertex, uv));

	fp_shader = load_shader("shaders/passes/final_pass.vert", "shaders/passes/final_pass.frag");
	if (fp_shader == NULL) return 1;
	return 0;
}

void final_pass_execute(render_pass* rp, render_context* context, render_targets* targets) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//glDisable(GL_DEPTH_TEST);

    shader_use(fp_shader);

	render_texture_bind(&targets->opaque_tex, 0);
	shader_set_int(fp_shader, "screen", 0);

    glBindVertexArray(fp_vao);
    glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_INT, 0);
}

void final_pass_resize(render_pass* rp, uint32_t width, uint32_t height) {
	rp->width = width;
	rp->height = height;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

void final_pass_deinit(render_pass* rp) {
	glDeleteBuffers(1, &fp_vbo);
	glDeleteBuffers(1, &fp_ebo);
	glDeleteVertexArrays(1, &fp_vao);
}

