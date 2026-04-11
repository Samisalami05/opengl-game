#include "debug_pass.h"
#include "core/shader.h"
#include "math/vec3.h"
#include "rendering/camera.h"
#include "rendering/pipeline.h"
#include "rendering/renderer.h"
#include "rendering/rendertexture.h"
#include "resourcemanager.h"
#include "util/arraylist.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    vec3* vertices;
    uint32_t* indices;
	uint32_t vertex_capacity;
    uint32_t vertex_count;
	uint32_t index_capacity;
    uint32_t index_count;
} debug_batch;

typedef struct {
	uint32_t vbo;
	uint32_t vao;
	uint32_t ebo;
} debug_buffer;

static debug_batch batch = {0};
static debug_buffer buffer = {0};

static shader* debug_shader = NULL;

uint32_t debug_pass_base() {
	return batch.vertex_count;
}

uint8_t debug_pass_add_vertices(vec3* vertices, uint32_t count) {
	uint8_t allocate = 0;
	while (batch.vertex_count + count > batch.vertex_capacity) {
		batch.vertex_capacity = batch.vertex_capacity == 0 ? 16 : batch.vertex_capacity * 4;
		allocate = 1;
	}

	if (allocate) {
		void* tmp = realloc(batch.vertices, batch.vertex_capacity * sizeof(vec3));
		if (tmp == NULL) {
			perror("debug pass: realloc");
			return 1;
		}
		batch.vertices = tmp;
	}

	memcpy(batch.vertices + batch.vertex_count, vertices, sizeof(vec3) * count);
	batch.vertex_count += count;

	return 0;
}

uint8_t debug_pass_add_indices(uint32_t* indices, uint32_t count) {
	uint8_t allocate = 0;
	while (batch.index_count + count > batch.index_capacity) {
		batch.index_capacity = batch.index_capacity == 0 ? 16 : batch.index_capacity * 4;
		allocate = 1;
	}

	if (allocate) {
		void* tmp = realloc(batch.indices, batch.index_capacity * sizeof(uint32_t));
		if (tmp == NULL) {
			perror("debug pass: realloc");
			return 1;
		}
		batch.indices = tmp;
	}

	memcpy(batch.indices + batch.index_count, indices, sizeof(uint32_t) * count);
	batch.index_count += count;

	return 0;
}

void debug_upload(debug_batch* batch, debug_buffer* buffer) { // TODO: do persistant mapping
    glBindVertexArray(buffer->vao);

    // --- VBO ---
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        batch->vertex_capacity * sizeof(vec3),
        NULL,
        GL_DYNAMIC_DRAW
    ); // orphan

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        batch->vertex_count * sizeof(vec3),
        batch->vertices
    );

    // --- EBO ---
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ebo);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        batch->index_capacity * sizeof(uint32_t),
        NULL,
        GL_DYNAMIC_DRAW
    );

    glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER,
        0,
        batch->index_count * sizeof(uint32_t),
        batch->indices
    );
}

uint8_t debug_pass_init(render_pass* rp, render_targets* targets) {
	strcpy(rp->name, "debug pass");
	rp->width = 640;
	rp->height = 480;
	rp->data = NULL;

	rp->execute = debug_pass_execute;
	rp->resize = debug_pass_resize;
	rp->deinit = debug_pass_deinit;

	debug_shader = load_shader("shaders/debug.vert", "shaders/debug.frag");
	if (debug_shader == NULL) return 1;

	glGenFramebuffers(1, &rp->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, rp->fbo);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
	    GL_COLOR_ATTACHMENT0,
	    GL_TEXTURE_2D,
	    targets->opaque_tex.handle,
	    0
	);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Debug pass: Framebuffer is not complete\n");
		return 1;
	}

	glGenVertexArrays(1, &buffer.vao);
	glBindVertexArray(buffer.vao);

	glGenBuffers(1, &buffer.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * batch.vertex_count, batch.vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &buffer.ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch.index_count * sizeof(uint32_t), batch.indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

	return 0;
}

void debug_pass_execute(render_pass* rp, render_context* context, render_targets* targets) {
	glBindFramebuffer(GL_FRAMEBUFFER, rp->fbo);
	glViewport(0, 0, rp->width, rp->height);

    //glClear(GL_COLOR_BUFFER_BIT);

	shader_use(debug_shader);
	mat4 proj = camera_proj(context->camera);
	mat4 view = camera_view(context->camera);

	shader_set_mat4(debug_shader, "projection", proj);
	shader_set_mat4(debug_shader, "view", view);

	debug_upload(&batch, &buffer);
	glDrawArrays(GL_LINES, 0, batch.index_count);

	// Clear batch
	batch.vertex_count = 0;
	batch.index_count = 0;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void debug_pass_resize(render_pass* rp, uint32_t width, uint32_t height) {
	rp->width = width;
	rp->height = height;
}

void debug_pass_deinit(render_pass* rp) {
	glDeleteFramebuffers(1, &rp->fbo);
}
