#ifndef LIGHTBUFFER_H
#define LIGHTBUFFER_H

#include "util/arraylist.h"
#include "light.h"
#include "core/buffers/ssbo.h"
#include "math/vec4.h"

typedef struct gpulight {
    vec4 color_intensity; // color     + intensity
    vec4 position_range;  // position  + range
    vec4 direction_type;  // direction + type (encoded)
} gpulight;

typedef struct lightbuffer {
	ssbo ssbo;
	arraylist lights;
} lightbuffer;

void lightbuffer_init(lightbuffer* lb);
void lightbuffer_deinit(lightbuffer* lb);
void lightbuffer_update(lightbuffer* lb, arraylist lights);

#endif
