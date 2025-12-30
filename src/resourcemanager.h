#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "shader.h"
#include "texture.h"

texture* load_texture(const char* path);
shader* load_shader(const char* vertex, const char* fragment);

#endif
