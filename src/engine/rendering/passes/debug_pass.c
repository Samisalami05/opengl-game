#include "debug_pass.h"
#include "allocator.h"
#include "core/shader.h"
#include "math/vec3.h"
#include "rendering/camera.h"
#include "rendering/pipeline.h"
#include "rendering/renderer.h"
#include "rendering/rendertexture.h"
#include "resourcemanager.h"
#include "util/arraylist.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
	vec3 pos;
	uint32_t color_id;
} debug_vertex;

typedef struct {
    debug_vertex* vertices;
	uint32_t vertex_capacity;
    uint32_t vertex_count;

    uint32_t* indices;
	uint32_t index_capacity;
    uint32_t index_count;

	vec4* colors;
	uint32_t color_capacity;
	uint32_t color_count;
} debug_batch;

typedef struct {
	uint32_t vbo;
	uint32_t vao;
	uint32_t ebo;
	uint32_t cbo;

	uint32_t gpu_vertex_capacity;
	uint32_t gpu_index_capacity;
	uint32_t gpu_color_capacity;
} debug_buffer;

static debug_batch batch = {0};
static debug_buffer buffer = {0};

static shader* debug_shader = NULL;

uint32_t debug_pass_base() {
	return batch.vertex_count;
}

uint8_t debug_change_color(vec4 color) {
	if (batch.color_count + 1 > batch.color_capacity) {
		batch.color_capacity = batch.color_capacity == 0 ? 4 : batch.color_capacity * 2;
		void* tmp = REALLOC(batch.colors, batch.color_capacity * sizeof(vec4));
		if (tmp == NULL) {
			perror("debug pass: realloc");
			return 1;
		}
		batch.colors = tmp;
	}

	batch.colors[batch.color_count] = color;
	batch.color_count++;
	return 0;
}

uint8_t debug_pass_add_vertices(vec3* vertices, uint32_t count) {
	uint8_t allocate = 0;
	while (batch.vertex_count + count > batch.vertex_capacity) {
		batch.vertex_capacity = batch.vertex_capacity == 0 ? 16 : batch.vertex_capacity * 4;
		allocate = 1;
	}

	if (allocate) {
		void* tmp = REALLOC(batch.vertices, batch.vertex_capacity * sizeof(debug_vertex));
		if (tmp == NULL) {
			perror("debug pass: realloc");
			return 1;
		}
		batch.vertices = tmp;
	}

	// Add color id to vertices
	debug_vertex verts[count];
	for (int i = 0; i < count; i++) {
		verts[i] = (debug_vertex){
			.pos = vertices[i],
			.color_id = batch.color_count - 1,
		};
	}

	memcpy(batch.vertices + batch.vertex_count, verts, sizeof(debug_vertex) * count);
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
		void* tmp = REALLOC(batch.indices, batch.index_capacity * sizeof(uint32_t));
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

static void resize_buffer(uint32_t buffer, GLenum target, size_t size) {
	glBindBuffer(target, buffer);

	glBufferData(
        target,
        size,
        NULL,
        GL_DYNAMIC_DRAW
    );
}

static void write_buffer(uint32_t buffer, GLenum target, size_t size, void* data) { // TODO: write to gpu pointer
	glBindBuffer(target, buffer);

	glBufferSubData(
        target,
        0,
        size,
        data
    );
}

static void debug_upload(debug_batch* batch, debug_buffer* buffer) { // TODO: do persistant mapping
	glBindVertexArray(buffer->vao);

    // --- VBO ---
	if (buffer->gpu_vertex_capacity < batch->vertex_count) {
		resize_buffer(buffer->vbo, GL_ARRAY_BUFFER, batch->vertex_count * sizeof(debug_vertex));
		buffer->gpu_vertex_capacity = batch->vertex_count;
	}
	write_buffer(buffer->vbo, GL_ARRAY_BUFFER, batch->vertex_count * sizeof(debug_vertex), batch->vertices);
    
	// --- EBO ---
	if (buffer->gpu_index_capacity < batch->index_count) {
		resize_buffer(buffer->ebo, GL_ELEMENT_ARRAY_BUFFER, batch->index_count * sizeof(uint32_t));
		buffer->gpu_index_capacity = batch->index_count;
	}
	write_buffer(buffer->ebo, GL_ELEMENT_ARRAY_BUFFER, batch->index_count * sizeof(uint32_t), batch->indices);

	// --- Colors ---
	if (buffer->gpu_color_capacity < batch->color_count) {
		resize_buffer(buffer->cbo, GL_SHADER_STORAGE_BUFFER, batch->color_count * sizeof(vec4));
		buffer->gpu_color_capacity = batch->color_count;
	}
	write_buffer(buffer->cbo, GL_SHADER_STORAGE_BUFFER, batch->color_count * sizeof(vec4), batch->colors);
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
	
	glGenBuffers(1, &buffer.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(debug_vertex), (void*)0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(debug_vertex), (void*)sizeof(vec3));

	glGenBuffers(1, &buffer.cbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer.cbo);

	// Set default color
	debug_change_color((vec4){1.0f, 1.0f, 1.0f, 1.0f});

	return 0;
}

void debug_pass_execute(render_pass* rp, render_context* context, render_targets* targets) {
	if (batch.index_count == 0 || batch.vertex_count == 0) return;

	glBindFramebuffer(GL_FRAMEBUFFER, rp->fbo);
	glViewport(0, 0, rp->width, rp->height);

	shader_use(debug_shader);
	mat4 proj = camera_proj(context->camera);
	mat4 view = camera_view(context->camera);

	shader_set_mat4(debug_shader, "projection", proj);
	shader_set_mat4(debug_shader, "view", view);

	debug_upload(&batch, &buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer.cbo);
	glDrawElements(GL_LINES, batch.index_count, GL_UNSIGNED_INT, 0);

	// Clear batch
	batch.vertex_count = 0;
	batch.index_count = 0;
	batch.color_count = 1; // Leave default color

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void debug_pass_resize(render_pass* rp, uint32_t width, uint32_t height) {
	rp->width = width;
	rp->height = height;
}

void debug_pass_deinit(render_pass* rp) {
	FREE(batch.colors);
	FREE(batch.indices);
	FREE(batch.vertices);
	glDeleteFramebuffers(1, &rp->fbo);
}
