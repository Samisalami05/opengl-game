#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"
#include "texture.h"
#include <stdint.h>

typedef enum material_type {
	MAT_DEFAULT,
	MAT_TEXTURE,
	MAT_LIT, // TODO: Implement these
	MAT_UNLIT,
} material_type;

typedef struct material {
	material_type type;
	shader shader;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} material;

void material_init(material* m, material_type type);
void material_use(material* m);

#endif
