#ifndef RENDERER_H
#define RENDERER_H

#include "modelloader.h"
#include "rendering/pipeline.h"
#include "scene.h"
#include "camera.h"
#include <stdint.h>

typedef struct {
	render_pipeline pipeline;
	render_context context;
} renderer;

void renderer_init(renderer* r, uint32_t width, uint32_t height);
void renderer_deinit(renderer* r);
void render(renderer* r, model* models, uint32_t count, camera* cam);

void render_model(model* m, camera* cam, vec3 pos, vec3 rot, vec3 scale);
void render_scene(scene* s);

#endif
