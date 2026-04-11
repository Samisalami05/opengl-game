#include "depth_pass.h"
#include "core/shader.h"
#include "rendering/pipeline.h"
#include "rendering/renderer.h"
#include "rendering/rendertexture.h"
#include "resourcemanager.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static shader* dp_shader = NULL;

static void shader_set_mvp(shader* s, camera* cam, vec3 pos, vec3 rot, vec3 scale) {
	mat4 model;
	mat4_identity(&model);
	mat4_translate(&model, pos);
	mat4_rotate(&model, rot);
	mat4_scale_v3(&model, scale);

	mat4 view = camera_view(cam);
	mat4 projection = camera_proj(cam);

	shader_set_mat4(s, "model", model);
	shader_set_mat4(s, "view", view);
	shader_set_mat4(s, "projection", projection);
}

uint8_t depth_pass_init(render_pass* rp, render_targets* targets) {
	strcpy(rp->name, "depth pass");
	rp->width = 640;
	rp->height = 480;
	rp->data = NULL;

	rp->execute = depth_pass_execute;
	rp->resize = depth_pass_resize;
	rp->deinit = depth_pass_deinit;

	dp_shader = load_shader("shaders/depth.vert", "shaders/depth.frag");
	if (dp_shader == NULL) return 1;

	glGenFramebuffers(1, &rp->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, rp->fbo);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
	    GL_DEPTH_ATTACHMENT,
	    GL_TEXTURE_2D,
	    targets->depth_tex.handle,
	    0
	);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Depth pass: Framebuffer is not complete\n");
		return 1;
	}
	return 0;
}

void depth_pass_execute(render_pass* rp, render_context* context, render_targets* targets) {
	glBindFramebuffer(GL_FRAMEBUFFER, rp->fbo);
	glViewport(0, 0, rp->width, rp->height);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    // Disable color output
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_DEPTH_BUFFER_BIT);

	shader_use(dp_shader);
	for (int i = 0; i < context->draw_count; i++) {
		model m = context->drawlist[i];

		for (int i = 0; i < m.mesh_count; i++) {
			shader_set_mvp(dp_shader, context->camera, (vec3){0}, (vec3){0}, (vec3){1.0f, 1.0f, 1.0f});

			mesh_use(&m.meshes[i]);

			glDrawElements(GL_TRIANGLES, m.meshes[i].index_count, GL_UNSIGNED_INT, 0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void depth_pass_resize(render_pass* rp, uint32_t width, uint32_t height) {
	rp->width = width;
	rp->height = height;
}

void depth_pass_deinit(render_pass* rp) {
	glDeleteFramebuffers(1, &rp->fbo);
}
