#ifndef RENDERER_H
#define RENDERER_H

#include "entity.h"
#include "core/mesh.h"
#include "modelloader.h"
#include "rendering/pipeline.h"
#include "scene.h"
#include "material.h"
#include "camera.h"
#include <stdint.h>

typedef struct {
	render_pipeline pipeline;
	uint32_t frame_index;
} renderer;

void render_model(model* m, camera* cam, vec3 pos, vec3 rot, vec3 scale);
void render_scene(scene* s);

#endif
