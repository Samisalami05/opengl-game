#ifndef VEC4_H
#define VEC4_H

#include "math/vec2.h"
#include "math/vec3.h"

typedef union {
	struct { float x, y, z, w; };
	struct { vec2 xy, zw; };
	struct { float __pad1; vec2 yz; float __pad2; };
	struct { vec3 xyz; float __pad3; };
	struct { float __pad4; vec3 yzw; };
	struct { float data[4]; };
} vec4;

#endif
