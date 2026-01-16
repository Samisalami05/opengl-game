#ifndef MODELLOADER_H
#define MODELLOADER_H

#include "util/arraylist.h"

typedef struct model {
	arraylist meshes;
} model;

void load_model(model* m, const char* file);

#endif
