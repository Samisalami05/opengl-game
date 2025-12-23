#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"

typedef struct material {
	shader shader;
} material;

void material_init(material* m, char* shader);

#endif
