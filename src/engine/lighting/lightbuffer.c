#include "lightbuffer.h"
#include "core/buffers/ssbo.h"

void lightbuffer_init(lightbuffer* lb) {
	///ssbo_init(lb->ssbo, sizeof(gpulight) * 100, GL_, unsigned int binding)
}

void lightbuffer_deinit(lightbuffer* lb);
void lightbuffer_update(lightbuffer* lb, arraylist lights);
