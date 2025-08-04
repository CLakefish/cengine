#ifndef GIZMO_RENDERER
#define GIZMO_RENDERER

#include "Gizmo.h"
#include "Shader.h"

#define GRID_WIDTH 80
#define GRID_LENGTH 80
#define GRID_SPACING 15.0f
#define GRID_FOG 4500
#define GRID_COLOR 1, 1, 1, 0.5

#define DIR_SPACING 5000

typedef struct GizmoRenderer {
	shader_t* shader;
	shader_t* instanceShader;

	gizmoInstance_t* instance;

	gizmo_t** gizmos;
	int count, capacity;
} GizmoRenderer;

extern GizmoRenderer gizmoRenderer;

void GizmoRenderer_Init(void);
void GizmoRenderer_Shutdown(void);
void GizmoRenderer_Render(const mat4x4_t* view, const mat4x4_t* proj, const vec3_t pos);
void GizmoRenderer_AddGizmo(gizmo_t* gizmo);
void GizmoRenderer_RemoveGizmo(gizmo_t* gizmo);

void GizmoRenderer_GenerateGrid(const vec3_t position);
void GizmoRenderer_GenerateDir(const vec3_t position);

#endif