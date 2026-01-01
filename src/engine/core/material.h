#ifndef MATERIAL_H
#define MATERIAL_H

#include "core/shader.h"
#include "core/texture.h"
#include "math/ivec2.h"
#include <stdint.h>

typedef enum material_type {
	MAT_COLOR_LIT,
	MAT_COLOR_UNLIT,
	MAT_TEXTURE_LIT,
	MAT_TEXTURE_UNLIT,
	MAT_UNINITIALIZED,
} material_type;

typedef struct material {
	material_type type;
	shader* shader;

	texture* albedo_tex;
	//texture* normal_tex;
	//texture* roughness_tex;
	//texture* metallic_tex;
	//texture* glossiness_tex;
	//texture* specular_tex;
	
	vec3 color;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;

	vec3 emission; // TODO: Implement this
	float emission_str;

	vec2 tiling;
} material;

void material_init(material* m, material_type type);
void material_use(material* m);
void material_deinit(material* m);

#endif
