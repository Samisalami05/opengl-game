#include "rendering.h"
#include "camera.h"
#include "entity.h"
#include "mat4.h"
#include "material.h"
#include "mesh.h"
#include "shader.h"
#include <glad/glad.h>

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

void render_scene(scene* s) {
	for (int i = 0; i < s->entities.count; i++) {
		entity* e = &((entity*)s->entities.data)[i];
		render_entity(e, &s->cam);
	}
	// TODO: Render skybox
}
