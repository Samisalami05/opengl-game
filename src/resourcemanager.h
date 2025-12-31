#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "core/shader.h"
#include "core/texture.h"

void resource_manager_init();
void resource_manager_deinit();

texture* load_texture(const char* path);
shader* load_shader(const char* vertex, const char* fragment);

#endif
