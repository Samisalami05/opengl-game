#include "material.h"
#include "shader.h"
#include <stdio.h>

void material_init(material* m, material_type type) {
	m->type = type;
	
	m->ambient = (vec3){0.2f, 0.0f, 0.0f};
	m->diffuse = (vec3){1.0f, 0.5f, 0.31f};
	m->specular = (vec3){0.5f, 0.5f, 0.5f};
	m->shininess = 32.0f;

	switch (m->type) {
		case MAT_DEFAULT:
			shader_init(&m->shader, "shaders/basic.vert", "shaders/basic.frag");
			break;
		default:
			fprintf(stderr, "Material type not implemented\n");
	}
}

void material_use(material* m) {
	shader_use(m->shader);

	shader_set_vec3(m->shader, "mat.ambient", m->ambient);
	shader_set_vec3(m->shader, "mat.diffuse", m->diffuse);
	shader_set_vec3(m->shader, "mat.specular", m->specular);
	shader_set_float(m->shader, "mat.shininess", m->shininess);
}
