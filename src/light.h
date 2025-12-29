#ifndef LIGHT_H
#define LIGHT_H

#include "vec3.h"

typedef enum ligthtype {
	LIGHT_GLOBAL,
	LIGHT_POINT,
	//LIGHT_AREA,
} ligthtype;

typedef struct globallight {
	vec3 dir;
} globallight;

typedef struct pointlight {
	vec3 position;
	float range;
} pointlight;

//typedef struct arealight {
//	float radius;
//} arealight;

typedef struct light {
	ligthtype type;
	float intensity;
	float range;
	vec3 color;
	vec3 dir;
	vec3 position;
} light;

void light_init_global(light* light, const vec3 dir);
void light_init_point(light* light, const vec3 position);

#endif
