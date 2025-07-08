#ifndef TRANSFORM
#define TRANSFORM

#include "Vector3.h"

typedef struct transform_t {
	vec3_t position, rotation, scale;
} transform_t;

#endif