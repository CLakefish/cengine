#ifndef TRANSFORM
#define TRANSFORM

#include "Vector3.h"

typedef struct transform_t {
	vec3_t position, rotation, scale;
	size_t ID;
} transform_t;

static vec3_t Transform_GetForward(const transform_t* t) {
	float pitch = DEG2RAD * t->rotation.x;
	float yaw = DEG2RAD * t->rotation.y;

	return (vec3_t) {
		cosf(pitch)* sinf(yaw),
		sinf(pitch),
		cosf(pitch)* cosf(yaw),
	};
};

static vec3_t Transform_GetRight(const transform_t* t) {
	vec3_t forward = Transform_GetForward(t);
	vec3_t worldUp = { 0.0f, 1.0f, 0.0f };
	vec3_t right   = vec3_Normalize(vec3_Cross(forward, worldUp));
	vec3_t up      = vec3_Normalize(vec3_Cross(right,   forward));

	float roll = DEG2RAD * t->rotation.z;

	vec3_t rotatedRight = vec3_Normalize((vec3_t) {
		right.x * cosf(roll) + up.x * sinf(roll),
		right.y * cosf(roll) + up.y * sinf(roll),
		right.z * cosf(roll) + up.z * sinf(roll),
	});

	return rotatedRight;
};

static vec3_t Transform_GetUp(const transform_t* t) {
	vec3_t forward = Transform_GetForward(t);
	vec3_t right   = Transform_GetRight(t);

	return vec3_Normalize(vec3_Cross(right, forward));
};

// Dumb, use rotation matrix conversion later
// Turn fwd into identity matrix
// invert matrix
// multiply matrix by unit vector.
static void Transform_SetForward(transform_t* t, const vec3_t fwd) {
}

#endif