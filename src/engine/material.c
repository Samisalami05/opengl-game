#include "material.h"
#include "resourcemanager.h"
#include "core/shader.h"
#include "core/texture.h"
#include <stdio.h>
#include <stdlib.h>

void material_init(material* m, material_type type) {
	m->type = type;

	m->color = (vec3){1.0f, 1.0f, 1.0f};
	
	m->ambient = (vec3){1.0f, 1.0f, 1.0f};
	m->diffuse = (vec3){1.0f, 1.0f, 1.0f};
	m->specular = (vec3){1.0f, 1.0f, 1.0f};
	m->shininess = 1.0f;

	switch (m->type) {
		case MAT_COLOR_LIT:
			m->shader = load_shader("shaders/basic.vert", "shaders/color_lit.frag");
			break;
		case MAT_COLOR_UNLIT:
			m->shader = load_shader("shaders/basic.vert", "shaders/color_unlit.frag");
			break;
		case MAT_TEXTURE_LIT:
			m->shader = load_shader("shaders/basic.vert", "shaders/texture_lit.frag");
			m->albedo_tex = load_texture("assets/white.png");
			m->tiling = (vec2){1.0f, 1.0f};
			break;
		case MAT_TEXTURE_UNLIT:
			m->shader = load_shader("shaders/basic.vert", "shaders/texture_unlit.frag");
			m->albedo_tex = load_texture("assets/white.png");
			m->tiling = (vec2){1.0f, 1.0f};
			break;
		case MAT_UNINITIALIZED:
			m->shader = NULL;
			m->albedo_tex = NULL;
			break;
		default:
			fprintf(stderr, "Material type not implemented\n");
	}
}

static void bind_material_lighting(material* m) {
	shader_set_vec3(m->shader, "mat.ambient", m->ambient);
	shader_set_vec3(m->shader, "mat.diffuse", m->diffuse);
	shader_set_vec3(m->shader, "mat.specular", m->specular);
	shader_set_float(m->shader, "mat.shininess", m->shininess);

	shader_set_vec3(m->shader, "mat.emission", m->emission);
	shader_set_float(m->shader, "mat.emission_str", m->emission_str);
}

void material_use(material* m) {
	shader_use(m->shader);

	switch (m->type) {
		case MAT_COLOR_LIT:
			bind_material_lighting(m);	
		case MAT_COLOR_UNLIT:
			shader_set_vec3(m->shader, "mat.color", m->color);
			break;
		case MAT_TEXTURE_LIT:
			bind_material_lighting(m);
		case MAT_TEXTURE_UNLIT:
			shader_set_int(m->shader, "mat.albedo_tex", 0);
			shader_set_vec3(m->shader, "mat.color", m->color);
			shader_set_vec2(m->shader, "mat.tiling", m->tiling);
			texture_use(m->albedo_tex, 0);
			break;
		default:
			fprintf(stderr, "Material type not implemented\n");
	}
}

void material_deinit(material* m) {
	shader_deinit(m->shader);
	m->albedo_tex = NULL;
}
