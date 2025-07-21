#ifndef GIZMO
#define GIZMO

#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Transform.h"
#include "Vector3.h"
#include "Matrix4x4.h"

typedef struct gizmoVertex_t {
	vec3_t position;
	vec3_t color;
} gizmoVertex_t;

/// <summary>
/// REMINDER FOR THE MF WHO WROTE THIS (me :))
/// This system is separate from the rest of the engine, it probably should share more similarities w/the core itself
/// This system is also very unoptimized, it could be fixed with a lot of things (you could figure it out, instancing, not having to generate the same vertices over and over, etc...)
/// </summary>
typedef struct gizmo_t {
	gizmoVertex_t* vertices;
	uint32_t vertexCount;
	uint8_t render;

	transform_t transform;
	GLuint VAO, VBO;
} gizmo_t;

typedef struct gizmoInstance_t {
	gizmo_t* gizmo;

	mat4x4_t* transformations;
	int count, capacity;

	GLuint VAO, VBO;
} gizmoInstance_t;

void Gizmo_Bind(gizmo_t* gizmo);
gizmo_t* Gizmo_CreateLine(const vec3_t start, const vec3_t end, const vec3_t color);
gizmo_t* Gizmo_CreateRay(const vec3_t start, const vec3_t end, const vec3_t color);
gizmo_t* Gizmo_CreateBox(const vec3_t position, const vec3_t scale, const vec3_t color);

gizmoInstance_t* Gizmo_CreateBoxInstance(const vec3_t position, const vec3_t scale, const vec3_t color, const size_t totalInstances, mat4x4_t* transformations);

#endif