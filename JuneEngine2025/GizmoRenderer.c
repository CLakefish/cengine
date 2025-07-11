#include "GizmoRenderer.h"
#include "FileIO.h"
#include "Matrix4x4.h"

#include <string.h>

static GizmoRenderer gizmoRenderer = { 0 };

static void GizmoRenderer_Expand(void) {
	int initCap = gizmoRenderer.capacity;
	gizmoRenderer.capacity *= 2;

	gizmo_t** g = (gizmo_t**)realloc(gizmoRenderer.gizmos, sizeof(gizmo_t*) * gizmoRenderer.capacity);
	if (!g) {
		perror("realloc");
		return;
	}

	memset(g + initCap, 0, (gizmoRenderer.capacity - initCap) * sizeof(gizmo_t*));

	gizmoRenderer.gizmos = g;
}

void GizmoRenderer_Init(void) {
	gizmoRenderer.gizmos = (gizmo_t**)calloc(1, sizeof(gizmo_t*));

	if (!gizmoRenderer.gizmos) {
		perror("malloc");
		return;
	}

	gizmoRenderer.capacity = 1;
	gizmoRenderer.count	   = 0;

	gizmoRenderer.shader = Shader_Init(FileToString("gizmo.vert"), FileToString("gizmo.frag"));

	int cell = GRID_WIDTH * GRID_LENGTH;
	int inds = cell * 4;

	gizmoRenderer.grid = (gizmo_t*)malloc(sizeof(gizmo_t));
	gizmoRenderer.grid->vertices = (gizmoVertex_t*)calloc(inds, sizeof(gizmoVertex_t));

	if (!gizmoRenderer.grid) {
		perror("malloc");
		return;
	}

	gizmoRenderer.grid->count = inds;
	Gizmo_Bind(gizmoRenderer.grid);
}
/// <summary>
/// This has the funny 6001 in it :(
/// </summary>
/// <param name=""></param>
void GizmoRenderer_Shutdown(void) {
	Shader_Clear(gizmoRenderer.shader);

	free(gizmoRenderer.grid->vertices);
	free(gizmoRenderer.grid);

	for (int i = 0; i < gizmoRenderer.count; ++i) {
		gizmo_t* g = gizmoRenderer.gizmos[i];

		glDeleteVertexArrays(1, &g->VAO); 
		glDeleteBuffers(1, &g->VBO);

		// MSVC wrongly reports this as using uninitialised variables.
		// sHUT UP MSVC IM GONNA KILL YOU ISTG
		#pragma warning(push)
		#pragma warning(disable: 6001)
		free(g->vertices); 
		#pragma warning(pop)
		free(g);
	}

	free(gizmoRenderer.gizmos);
}

void GizmoRenderer_Render(const mat4x4_t* view, const mat4x4_t* proj, const vec3_t pos){
	Shader_Use(gizmoRenderer.shader);

	Shader_SetMat4(gizmoRenderer.shader, "view", view);
	Shader_SetMat4(gizmoRenderer.shader, "proj", proj);

	mat4x4_t id = mat_Identity();
	Shader_SetMat4(gizmoRenderer.shader, "model", &id);

	GizmoRenderer_GenerateGrid(pos);

	glBindBuffer(GL_ARRAY_BUFFER, gizmoRenderer.grid->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gizmoVertex_t) * gizmoRenderer.grid->count, gizmoRenderer.grid->vertices, GL_STATIC_DRAW);
	glBindVertexArray(gizmoRenderer.grid->VAO);
	glDrawArrays(GL_LINES, 0, gizmoRenderer.grid->count);

	for (int i = 0; i < gizmoRenderer.count; ++i) {
		gizmo_t* gizmo = gizmoRenderer.gizmos[i];
		if (!gizmo->render) continue;

		//glBufferData(GL_ARRAY_BUFFER, sizeof(gizmoVertex_t) * gizmo->count, gizmo->vertices, GL_DYNAMIC_DRAW);

		mat4x4_t model = mat_Identity();
		model = mat_Scale(model, gizmo->transform.scale);
		model = mat_Rot(model, gizmo->transform.rotation);
		model = mat_Translate(model, gizmo->transform.position);

		Shader_SetMat4(gizmoRenderer.shader, "model", &model);

		glBindVertexArray(gizmo->VAO);
		glDrawArrays(GL_LINES, 0, gizmo->count);
	}
}

// NEEDS TO BE FIXED :)
void GizmoRenderer_AddGizmo(gizmo_t* gizmo) {
	if (gizmoRenderer.count >= gizmoRenderer.capacity) GizmoRenderer_Expand();

	gizmoRenderer.gizmos[gizmoRenderer.count] = gizmo;
	gizmoRenderer.count++;
}

// Add in shrinking
void GizmoRenderer_RemoveGizmo(gizmo_t* gizmo) {
	if (gizmo == NULL) {
		return;
	}

	for (int i = 0; i < gizmoRenderer.count; ++i) {
		if (memcmp(gizmo, gizmoRenderer.gizmos[i], sizeof(gizmo_t)) != 0) continue;

		for (int j = i; j < gizmoRenderer.count - 1; ++j) {
			gizmoRenderer.gizmos[j] = gizmoRenderer.gizmos[j + 1];
		}

		gizmoRenderer.count--;

		glDeleteVertexArrays(1, &gizmo->VAO);
		glDeleteBuffers(1, &gizmo->VBO);

		free(gizmo->vertices);
		free(gizmo);
		return;
	}
}

static void GizmoRenderer_CreateChunk(const vec3_t pos, int* index, const int x, const int z, const float xOffset, const float zOffset) {
	vec3_t aZ = (vec3_t){ (x * GRID_SPACING) - xOffset, 0, (z * GRID_SPACING) - zOffset, 1 };
	vec3_t bZ = (vec3_t){ (x * GRID_SPACING) - xOffset, 0, ((z + 1) * GRID_SPACING) - zOffset, 1 };

	//float fogA = 1.0f - (vec3_Distance(vec3_Mult(pos, -1), aZ) / GRID_FOG);
	//float fogB = 1.0f - (vec3_Distance(vec3_Mult(pos, -1), bZ) / GRID_FOG);
	float fogA = LERP(1.0f, 0.0f, vec3_Distance(pos, aZ) / GRID_FOG);
	float fogB = LERP(1.0f, 0.0f, vec3_Distance(pos, bZ) / GRID_FOG);
	vec3_t colorA = (vec3_t){ GRID_COLOR, fogA };
	vec3_t colorB = (vec3_t){ GRID_COLOR, fogB };

	int idx = *index;
	gizmoRenderer.grid->vertices[idx++] = (gizmoVertex_t){ aZ, colorA };
	gizmoRenderer.grid->vertices[idx++] = (gizmoVertex_t){ bZ, colorB }; // errors here

	vec3_t aX = (vec3_t){ (x * GRID_SPACING) - xOffset,	      0, (z * GRID_SPACING) - zOffset, 1 };
	vec3_t bX = (vec3_t){ ((x + 1) * GRID_SPACING) - xOffset, 0, (z * GRID_SPACING) - zOffset, 1 };

	fogA = LERP(1.0f, 0.0f, vec3_Distance(pos, aX) / GRID_FOG);
	fogB = LERP(1.0f, 0.0f, vec3_Distance(pos, bX) / GRID_FOG);
	colorA = (vec3_t){ GRID_COLOR, fogA };
	colorB = (vec3_t){ GRID_COLOR, fogB };

	gizmoRenderer.grid->vertices[idx++] = (gizmoVertex_t){ aX, colorA };
	gizmoRenderer.grid->vertices[idx++] = (gizmoVertex_t){ bX, colorB };

	*index = idx;
}

void GizmoRenderer_GenerateGrid(vec3_t pos) {
	float xOffset  = (int)(pos.x / GRID_SPACING) * GRID_SPACING;
	float zOffset  = (int)(pos.z / GRID_SPACING) * GRID_SPACING;

	int halfWidth  = (int)(GRID_WIDTH / 2.0f);
	int halfLength = (int)(GRID_LENGTH / 2.0f);
	int index = 0;

	vec3_t scaledPos = (vec3_t){ pos.x, 0, pos.z };

	for (int x = -halfWidth; x < halfWidth; ++x) {
		for (int z = -halfLength; z < halfLength; ++z) {
			GizmoRenderer_CreateChunk(scaledPos, &index, x, z, xOffset, zOffset);
		}
	}
}