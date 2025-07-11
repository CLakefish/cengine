#ifndef GIZMO_RENDERER
#define GIZMO_RENDERER

#include "Gizmo.h"
#include "Shader.h"

#define GRID_WIDTH 100
#define GRID_LENGTH 100
#define GRID_SPACING 15.0f
#define GRID_FOG 450.0f
#define GRID_COLOR 0.4f, 0.5f, 0.5f

typedef struct GizmoRenderer {
	gizmo_t** gizmos;
	int count, capacity;

	gizmo_t* grid;

	shader_t* shader;
} GizmoRenderer;

extern GizmoRenderer gizmoRenderer;

void GizmoRenderer_Init			(void);
void GizmoRenderer_Shutdown		(void);
void GizmoRenderer_Render		(const mat4x4_t* view, const mat4x4_t* proj, const vec3_t pos);
void GizmoRenderer_AddGizmo		(gizmo_t* gizmo);
void GizmoRenderer_RemoveGizmo  (gizmo_t* gizmo);
void GizmoRenderer_GenerateGrid	(const vec3_t position);

#endif