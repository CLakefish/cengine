#ifndef TRANSFORM
#define TRANSFORM

#include "Vector3.h"

typedef struct transform_t {
	vec3_t position, rotation, scale;
} transform_t;

static vec3_t Transform_GetForward(const transform_t* t) {
	return (vec3_t) {
		cosf(t->rotation.z)* sinf(t->rotation.y),
		-sinf(t->rotation.z),
		cosf(t->rotation.z)* cosf(t->rotation.y),
	};
};

static vec3_t Transform_GetRight(const transform_t* t) {
	return (vec3_t) {
		cosf(t->rotation.y),
		0,
		-sinf(t->rotation.y),
	};
};

static vec3_t Transform_GetUp(const transform_t* t) {
	return (vec3_t) {
		sinf(t->rotation.z) * sinf(t->rotation.y),
		cosf(t->rotation.z),
		sinf(t->rotation.z) * cosf(t->rotation.y),
	};
};

// Dumb, use rotation matrix conversion later
// Turn fwd into identity matrix
// invert matrix
// multiply matrix by unit vector.
static void Transform_SetForward(transform_t* t, const vec3_t fwd) {

}

#endif