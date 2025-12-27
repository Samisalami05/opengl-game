#ifndef RENDERER_H
#define RENDERER_H

#include "entity.h"
#include "mesh.h"
#include "scene.h"
#include "material.h"
#include "camera.h"

void render_mesh(mesh* m, material* mat, camera* cam);
void render_entity(entity* e, camera* cam);
void render_scene(scene* s);

#endif
