#include "GizmoRenderer.h"
#include "FileIO.h"
#include "Matrix4x4.h"
#include "TimeManager.h"

#include <string.h>
#include <assert.h>

static GizmoRenderer gizmoRenderer = { 0 };

static gizmo_t* grid;
static gizmo_t* directionLines;

static void GizmoRenderer_Expand(void) {
	int initCap = gizmoRenderer.capacity;
	gizmoRenderer.capacity *= 2;

	gizmo_t** g = (gizmo_t**)realloc(gizmoRenderer.gizmos, sizeof(gizmo_t*) * gizmoRenderer.capacity);
	if (!g) {
		perror("realloc");
		return;
	}

	memset(g + initCap, 0, ((size_t)gizmoRenderer.capacity - initCap) * sizeof(gizmo_t*));

	gizmoRenderer.gizmos = g;
}

static void GizmoRenderer_RenderGrid(const vec3_t pos) {
	Shader_SetFloat(gizmoRenderer.shader, "fogDist", GRID_FOG);
	Shader_SetVec3(gizmoRenderer.shader, "camPos", &(vec3_t){0, 0, 0});

	float totalSpace = GRID_SPACING * 2.0f;
	grid->transform.position = (vec3_t){ (int)(pos.x / totalSpace) * totalSpace, 0, (int)(pos.z / totalSpace) * totalSpace };

	mat4x4_t model = mat_Identity();
	model = mat_Scale(model, grid->transform.scale);
	model = mat_Rot(model, grid->transform.rotation);
	model = mat_Translate(model, grid->transform.position);

	Shader_SetMat4(gizmoRenderer.shader, "model", &model);

	glBindVertexArray(grid->VAO);
	glDrawArrays(GL_LINES, 0, grid->vertexCount);
}

static void GizmoRenderer_CreateChunk(const vec3_t pos, int* index, const int x, const int z, const float xOffset, const float zOffset) {
	float xSpace = x * GRID_SPACING - xOffset;
	float zSpaze = z * GRID_SPACING - zOffset;

	vec3_t aZ = (vec3_t){ xSpace, 0, zSpaze, 1 };
	vec3_t bZ = (vec3_t){ xSpace, 0, ((z + 1) * GRID_SPACING) - zOffset, 1 };

	vec3_t color = (vec3_t){ GRID_COLOR };

	int idx = *index;
	grid->vertices[idx++] = (gizmoVertex_t){ aZ, color };
	grid->vertices[idx++] = (gizmoVertex_t){ bZ, color };

	vec3_t aX = (vec3_t){ xSpace,							  0, zSpaze, 1 };
	vec3_t bX = (vec3_t){ ((x + 1) * GRID_SPACING) - xOffset, 0, zSpaze, 1 };

	grid->vertices[idx++] = (gizmoVertex_t){ aX, color };
	grid->vertices[idx++] = (gizmoVertex_t){ bX, color };

	*index = idx;
}

void GizmoRenderer_Init(void) {
	gizmoRenderer.gizmos = (gizmo_t**)calloc(1, sizeof(gizmo_t*));

	if (!gizmoRenderer.gizmos) {
		perror("malloc");
		return;
	}

	gizmoRenderer.capacity = 1;
	gizmoRenderer.count = 0;

	// Directional lines
	int dirInd = 6;
	directionLines = (gizmo_t*)malloc(sizeof(gizmo_t));

	if (!directionLines) {
		perror("malloc\n");
		return;
	}

	directionLines->vertices = (gizmoVertex_t*)calloc(dirInd, sizeof(gizmoVertex_t));
	directionLines->vertexCount = dirInd;
	directionLines->transform = (transform_t){ {0},{0},{1,1,1,1} };
	directionLines->render = 1;

	Gizmo_Bind(directionLines);
	GizmoRenderer_GenerateDir((vec3_t) { 0 });
	GizmoRenderer_AddGizmo(directionLines);

	// Infinte grid
	int cell = GRID_WIDTH * GRID_LENGTH;
	int inds = cell * 4;

	grid = (gizmo_t*)malloc(sizeof(gizmo_t));

	if (!grid) {
		perror("malloc\n");
		return;
	}

	grid->vertices = (gizmoVertex_t*)calloc(inds, sizeof(gizmoVertex_t));

	if (!grid) {
		perror("malloc");
		return;
	}

	grid->vertexCount = inds;
	grid->transform = (transform_t){ (vec3_t) { 0 }, (vec3_t) { 0 }, (vec3_t) { 1,1,1,1 } };
	Gizmo_Bind(grid);
	GizmoRenderer_GenerateGrid((vec3_t) { 0, 0, 0 });

	size_t instances = 1000;
	size_t total = instances * instances;
	mat4x4_t* totalInstances = (mat4x4_t*)calloc(total, sizeof(mat4x4_t));
	mat4x4_t  identity		 = mat_Identity();

	if (!totalInstances) {
		perror("calloc\n");
		return;
	}

	float half = instances * 0.5f;
	for (int i = -half; i < half; ++i)
	{
		for (int j = -half; j < half; ++j)
		{
			size_t index = (i + half) * instances + (j + half);
			totalInstances[index] = mat_Transpose(mat_Translate(identity, (vec3_t) { i, tan(i) * tan(j), j }));
		}
	}

	gizmoRenderer.instance = Gizmo_CreateBoxInstance((vec3_t) { 0,0,0,1 }, (vec3_t) { 1,1,1,1 }, (vec3_t) { 0.5f,0.25f,0.5f,0.25f }, total, totalInstances);

	// Shader
	gizmoRenderer.shader		 = Shader_Init(FileToString("gizmo.vert"),		   FileToString("gizmo.frag"));
	gizmoRenderer.instanceShader = Shader_Init(FileToString("gizmoInstance.vert"), FileToString("gizmo.frag"));
}

void GizmoRenderer_Shutdown(void) {
	Shader_Clear(gizmoRenderer.shader);
	Shader_Clear(gizmoRenderer.instanceShader);

	free(grid->vertices);
	free(grid);

	for (int i = 0; i < gizmoRenderer.count; ++i) {
		gizmo_t* g = gizmoRenderer.gizmos[i];
		if (g == NULL) continue;

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

	glDeleteVertexArrays(1, &gizmoRenderer.instance->gizmo->VAO);
	glDeleteBuffers(1, &gizmoRenderer.instance->gizmo->VBO);

	glDeleteVertexArrays(1, &gizmoRenderer.instance->VAO);
	glDeleteBuffers(1, &gizmoRenderer.instance->VBO);

	free(gizmoRenderer.instance->gizmo->vertices);
	free(gizmoRenderer.instance->gizmo);
	free(gizmoRenderer.instance->transformations);
	free(gizmoRenderer.instance);

	free(gizmoRenderer.gizmos);
}

void GizmoRenderer_Render(const mat4x4_t* view, const mat4x4_t* proj, const vec3_t pos) {
	Shader_Use(gizmoRenderer.instanceShader);

	Shader_SetMat4(gizmoRenderer.instanceShader, "view", view);
	Shader_SetMat4(gizmoRenderer.instanceShader, "proj", proj);

	glBindVertexArray(gizmoRenderer.instance->gizmo->VAO);
	glDrawArrays(GL_LINES, 0, gizmoRenderer.instance->gizmo->vertexCount);

	glBindVertexArray(gizmoRenderer.instance->VAO);
	glDrawArraysInstanced(GL_LINES, 0, gizmoRenderer.instance->gizmo->vertexCount, gizmoRenderer.instance->count);

	Shader_Use(gizmoRenderer.shader);

	Shader_SetMat4(gizmoRenderer.shader, "view", view);
	Shader_SetMat4(gizmoRenderer.shader, "proj", proj);

	GizmoRenderer_RenderGrid(pos);

	Shader_SetFloat(gizmoRenderer.shader, "fogDist", 1000000);
	Shader_SetVec3(gizmoRenderer.shader, "camPos", &pos);

	glDisable(GL_DEPTH_TEST);

	GizmoRenderer_GenerateDir(pos);

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
		glDrawArrays(GL_LINES, 0, gizmo->vertexCount);
	}

	glEnable(GL_DEPTH_TEST);
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

void GizmoRenderer_GenerateDir(const vec3_t position) {
	directionLines->vertices[0] = (gizmoVertex_t){ {position.x + DIR_SPACING, 0, 0,1}, {1,0,0,1} };
	directionLines->vertices[1] = (gizmoVertex_t){ {position.x - DIR_SPACING, 0, 0,1}, {1,0,0,1} };

	directionLines->vertices[2] = (gizmoVertex_t){ {0, 0, position.z - DIR_SPACING}, {0,0,1,1} };
	directionLines->vertices[3] = (gizmoVertex_t){ {0, 0, position.z + DIR_SPACING}, {0,0,1,1} };

	directionLines->vertices[4] = (gizmoVertex_t){ {0, position.y - DIR_SPACING, 0}, {0,1,0,1} };
	directionLines->vertices[5] = (gizmoVertex_t){ {0, position.y + DIR_SPACING, 0}, {0,1,0,1} };

	glBindBuffer(GL_ARRAY_BUFFER, directionLines->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gizmoVertex_t) * directionLines->vertexCount, directionLines->vertices, GL_DYNAMIC_DRAW);
}

void GizmoRenderer_GenerateGrid(vec3_t pos) {
	float xOffset = (int)(-pos.x / GRID_SPACING) * GRID_SPACING;
	float zOffset = (int)(-pos.z / GRID_SPACING) * GRID_SPACING;

	int halfWidth = (int)(GRID_WIDTH / 2.0f);
	int halfLength = (int)(GRID_LENGTH / 2.0f);
	int index = 0;

	vec3_t scaledPos = (vec3_t){ pos.x, 0, pos.z };

	for (int x = -halfWidth; x < halfWidth; ++x) {
		for (int z = -halfLength; z < halfLength; ++z) {
			GizmoRenderer_CreateChunk(scaledPos, &index, x, z, xOffset, zOffset);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, grid->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gizmoVertex_t) * grid->vertexCount, grid->vertices, GL_DYNAMIC_DRAW);
}