#ifndef CAMERA
#define CAMERA

#include "Transform.h"

typedef struct camera_t {
	transform_t transform;
	vec3_t forward, up, right;
	float FOV, nearPlane, farPlane;
} camera_t;

#endif // !CAMERA
