#ifndef LIGHTBUFFER_H
#define LIGHTBUFFER_H

#include "arraylist.h"
#include "light.h"
#include "ssbo.h"

typedef struct lightbuffer {
	ssbo ssbo;
	arraylist lights;
} lightbuffer;



#endif
