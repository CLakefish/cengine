#include "stdinc.h"

#include "Matrix4x4.h"
#include "GraphicsManager.h"
#include "Trace.h"

GraphicsManager* Graphics_Init(void) {
	GraphicsManager* m = (GraphicsManager*)malloc(sizeof(GraphicsManager));

	if (!m) {
		Trace_Log(TR_ERROR, "Unable to initialize graphics manager through malloc call!");
		perror("Unable to initialize graphics manager through malloc call!");
		return NULL;
	}

	GLFWwindow* wind = glfwCreateWindow(1080, 800, "test", NULL, NULL);
	if (!wind) {
		Trace_Log(TR_ERROR, "Unable to initialize graphics manager window!");
		perror("Unable to initialize graphics manager window!");
		return NULL;
	}

	m->window = wind;

	glfwMakeContextCurrent(m->window);

	return m;
}

void Graphics_SetupGLAD(void) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glCullFace(GL_FRONT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Graphics_Clear(GraphicsManager* m) {
	glClearColor(0.1f, 0, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics_Render(GraphicsManager* m) {
	Graphics_Clear(m);

	// mat4x4_t proj = mat_Perspective(m->camera->FOV * DEG2RAD, 1.77777778f, m->camera->nearPlane, m->camera->farPlane);
	// mat4x4_t view = mat_LookAt     (m->camera->transform->position, m->camera->forward, m->camera->right, m->camera->up);

	// Shit goes here

	glfwSwapBuffers(m->window);
}

void Graphics_Shutdown(GraphicsManager* m) {
	glfwDestroyWindow(m->window);
	m->window = NULL;

	free(m);
}