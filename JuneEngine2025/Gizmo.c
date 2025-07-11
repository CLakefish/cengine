#include "Gizmo.h"
#include <string.h>

gizmoVertex_t cubeVertices[24] = {
	{{ 0.5f,  0.5f,  0.5f}, {1,1,1,1}},
    {{-0.5f,  0.5f,  0.5f}, {1,1,1,1}},
    {{-0.5f,  0.5f,  0.5f}, {1,1,1,1}},
    {{-0.5f, -0.5f,  0.5f}, {1,1,1,1}},
    {{-0.5f, -0.5f,  0.5f}, {1,1,1,1}},
    {{ 0.5f, -0.5f,  0.5f}, {1,1,1,1}},
    {{ 0.5f, -0.5f,  0.5f}, {1,1,1,1}},
    {{ 0.5f,  0.5f,  0.5f}, {1,1,1,1}},
    {{-0.5f, -0.5f,  0.5f}, {1,1,1,1}},
    {{-0.5f, -0.5f, -0.5f}, {1,1,1,1}},
    {{ 0.5f, -0.5f,  0.5f}, {1,1,1,1}},
    {{ 0.5f, -0.5f, -0.5f}, {1,1,1,1}},
    {{-0.5f, -0.5f, -0.5f}, {1,1,1,1}},
    {{-0.5f,  0.5f, -0.5f}, {1,1,1,1}},
    {{ 0.5f, -0.5f, -0.5f}, {1,1,1,1}},
    {{ 0.5f,  0.5f, -0.5f}, {1,1,1,1}},
    {{-0.5f, -0.5f, -0.5f}, {1,1,1,1}},
    {{ 0.5f, -0.5f, -0.5f}, {1,1,1,1}},
    {{-0.5f,  0.5f, -0.5f}, {1,1,1,1}},
    {{ 0.5f,  0.5f, -0.5f}, {1,1,1,1}},
    {{-0.5f,  0.5f, -0.5f}, {1,1,1,1}},
    {{-0.5f,  0.5f,  0.5f}, {1,1,1,1}},
    {{ 0.5f,  0.5f, -0.5f}, {1,1,1,1}},
    {{ 0.5f,  0.5f,  0.5f}, {1,1,1,1}}
};

void Gizmo_Bind(gizmo_t* l) {
	l->render = 1;

	glGenVertexArrays(1, &l->VAO);
	glBindVertexArray(l->VAO);

	glGenBuffers(1, &l->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, l->VBO);

	// Might be wrong...
	glBufferData(GL_ARRAY_BUFFER, sizeof(gizmoVertex_t) * l->count, l->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gizmoVertex_t), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(gizmoVertex_t), (void*)offsetof(gizmoVertex_t, color));
	glEnableVertexAttribArray(1);
}

gizmo_t* Gizmo_CreateLine(const vec3_t start, const vec3_t end, const vec3_t color) {
	gizmo_t* l = (gizmo_t*)calloc(1, sizeof(gizmo_t));
	if (!l) {
		perror("malloc");
		return NULL;
	}

	l->count = 2;
	l->vertices = (gizmoVertex_t*)calloc(l->count, sizeof(gizmoVertex_t));
	if (!l->vertices) {
		perror("malloc");
		free(l);
		return NULL;
	}

	l->transform = (transform_t){ (vec3_t){0,0,0}, (vec3_t) { 0,0,0,0 }, (vec3_t) { 1,1,1 }};

	vec3_t col = (vec3_t){ color.x, color.y, color.z, color.w == 0 ? 1 : color.w };

	l->vertices[0] = (gizmoVertex_t){ start, col };
	l->vertices[1] = (gizmoVertex_t){ end,   col };

	Gizmo_Bind(l);

	return l;
}

gizmo_t* Gizmo_CreateRay(const vec3_t point, const vec3_t dir, const vec3_t color) {
	return Gizmo_CreateLine(point, vec3_Add(point, dir), color);
}

gizmo_t* Gizmo_CreateBox(const vec3_t position, const vec3_t scale, const vec3_t desiredColor) {
	gizmo_t* l = (gizmo_t*)calloc(1, sizeof(gizmo_t));
	if (!l) {
		perror("malloc");
		return NULL;
	}

	l->count = 24;
	l->vertices = (gizmoVertex_t*)calloc(l->count, sizeof(gizmoVertex_t));
	if (!l->vertices) {
		perror("malloc");
		free(l);
		return NULL;
	}

	l->transform = (transform_t){position, (vec3_t) { 0,0,0,0 }, scale};

	vec3_t color    = (vec3_t){ desiredColor.x, desiredColor.y, desiredColor.z, desiredColor.w == 0 ? 1 : desiredColor.w };

	memcpy(l->vertices, cubeVertices, sizeof(gizmoVertex_t) * 24);
	for (int i = 0; i < 24; ++i) l->vertices[i].color = color;

	Gizmo_Bind(l);

	return l;
}