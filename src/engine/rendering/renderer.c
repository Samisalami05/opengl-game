#include "renderer.h"
#include "core/cubemap.h"
#include "skybox.h"
#include "util/arraylist.h"
#include "camera.h"
#include "entity.h"
#include "lighting/light.h"
#include "math/mat4.h"
#include "material.h"
#include "core/mesh.h"
#include "core/shader.h"
#include <glad/glad.h>
#include <stdio.h>

void render_mesh(mesh* m, material* mat, camera* cam) {
	material_use(mat);
	shader_set_vec3(mat->shader, "view_pos", cam->pos);
	mesh_use(m);

	mat4 view = camera_view(cam);
	mat4 projection = camera_proj(cam);

	shader_set_mat4(mat->shader, "view", view);
	shader_set_mat4(mat->shader, "projection", projection);

	glDrawElements(GL_TRIANGLES, m->index_count, GL_UNSIGNED_INT, 0);
}

void render_entity(entity* e, camera* cam) {
	material_use(e->mat);

	mat4 model;
	mat4_identity(&model);
	mat4_translate(&model, e->position);
	mat4_rotate(&model, e->rotation);
	mat4_scale_v3(&model, e->scale);

	shader_set_mat4(e->mat->shader, "model", model);

	render_mesh(e->mesh, e->mat, cam);
}

void render_skybox(skybox* sb, camera* cam) {
	glDepthMask(GL_FALSE);
	shader_use(sb->shader);

	mat4 view = camera_view_no_translate(cam);
	mat4 proj = camera_proj(cam);
	shader_set_mat4(sb->shader, "view", view);
	shader_set_mat4(sb->shader, "projection", proj);

	glBindVertexArray(sb->vao);
	cubemap_use(&sb->cubemap, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
}

void render_scene(scene* s) {
	render_skybox(&s->skybox, &s->cam);
	for (int i = 0; i < s->entities.count; i++) {
		entity* e = &((entity*)s->entities.data)[i];
		material_use(e->mat);
		shader_set_int(e->mat->shader, "light_count", s->lights.count);

		char* base = "lights";

		for (int j = 0; j < s->lights.count; j++) {
			light* light = arraylist_get(&s->lights, j);
			char location[50];
			sprintf(location, "%s[%d].%s", base, j, "type");
			shader_set_int(e->mat->shader, location, light->type);
			sprintf(location, "%s[%d].%s", base, j, "intensity");
			shader_set_float(e->mat->shader, location, light->intensity);
			sprintf(location, "%s[%d].%s", base, j, "range");
			shader_set_float(e->mat->shader, location, light->range);

			sprintf(location, "%s[%d].%s", base, j, "color");
			shader_set_vec3(e->mat->shader, location, light->color);
			sprintf(location, "%s[%d].%s", base, j, "dir");
			shader_set_vec3(e->mat->shader, location, light->dir);
			sprintf(location, "%s[%d].%s", base, j, "position");
			shader_set_vec3(e->mat->shader, location, light->position);
		}

		render_entity(e, &s->cam);
	}

	for (int i = 0; i < s->lights.count; i++) {
		light* light = arraylist_get(&s->lights, i);
		if (light->type == LIGHT_POINT) {
			s->light_entity.position = light->position;
			s->light_entity.mat->color = light->color;
			render_entity(&s->light_entity, &s->cam);
		}
	}
	// TODO: Render skybox
}
