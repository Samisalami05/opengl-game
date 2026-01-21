#include "opaque_pass.h"
#include "core/shader.h"
#include "lighting/light.h"
#include "rendering/pipeline.h"
#include "rendering/rendertexture.h"
#include "scenemanager.h"
#include <stdint.h>
#include <stdio.h>

static void shader_set_lights(material* mat, arraylist* lights) {
	shader_set_int(mat->shader, "light_count", lights->count);

	char* base = "lights";

	for (int j = 0; j < lights->count; j++) {
		light* light = arraylist_get(lights, j);
		char location[50];
		sprintf(location, "%s[%d].%s", base, j, "type");
		shader_set_int(mat->shader, location, light->type);
		sprintf(location, "%s[%d].%s", base, j, "intensity");
		shader_set_float(mat->shader, location, light->intensity);
		sprintf(location, "%s[%d].%s", base, j, "range");
		shader_set_float(mat->shader, location, light->range);

		sprintf(location, "%s[%d].%s", base, j, "color");
		shader_set_vec3(mat->shader, location, light->color);
		sprintf(location, "%s[%d].%s", base, j, "dir");
		shader_set_vec3(mat->shader, location, light->dir);
		sprintf(location, "%s[%d].%s", base, j, "position");
		shader_set_vec3(mat->shader, location, light->position);
	}

}

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

static void render_model(model* m, camera* cam, vec3 pos, vec3 rot, vec3 scale) {
	for (int i = 0; i < m->mesh_count; i++) {
		material* mat = &m->materials[m->mesh_mat_indices[i]];
		
		material_use(mat);

		shader_set_lights(mat, &sm_get_current_scene()->lights);
		shader_set_mvp(mat->shader, cam, pos, rot, scale);
		shader_set_vec3(mat->shader, "view_pos", cam->pos);

		mesh_use(&m->meshes[i]);

		glDrawElements(GL_TRIANGLES, m->meshes[i].index_count, GL_UNSIGNED_INT, 0);
	}
}

void opaque_pass_init(render_pass* rp, render_targets* targets) {
	rp->name = "opague pass";
	rp->width = 640;
	rp->height = 480;
	rp->data = NULL;

	rp->execute = opaque_pass_execute;
	rp->resize = opaque_pass_resize;
	rp->deinit = opaque_pass_deinit;

	glGenFramebuffers(1, &rp->fbo);
	
	glBindFramebuffer(GL_FRAMEBUFFER, rp->fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targets->opaque_tex.handle, 0);

	glFramebufferTexture2D(
	   GL_FRAMEBUFFER,
	    GL_DEPTH_ATTACHMENT,
	    GL_TEXTURE_2D,
	    targets->depth_tex.handle,
	    0
	);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Opague pass: Framebuffer is not complete\n");
		return;
	}
}

void opaque_pass_execute(render_pass* rp, render_context* context, render_targets* targets) {
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, rp->fbo);
	
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < context->draw_count; i++) {
		render_model(&context->drawlist[i], context->camera, (vec3){0}, (vec3){0}, (vec3){1.0f, 1.0f, 1.0f});
	}
}

void opaque_pass_resize(render_pass* rp, uint32_t width, uint32_t height) {
	rp->width = width;
	rp->height = height;
	glBindFramebuffer(GL_FRAMEBUFFER, rp->fbo);
	glViewport(0, 0, width, height);
}

void opaque_pass_deinit(render_pass* rp) {
	glDeleteFramebuffers(1, &rp->fbo);
}
