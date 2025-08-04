#ifndef GRAPHICS_MANAGER
#define GRAPHICS_MANAGER

#include <GLFW/glad.h>
#include <GLFW/glfw3.h>

#include "GizmoRenderer.h"
#include "Camera.h"

typedef struct GraphicsManager {
	GLFWwindow* window;
	camera_t camera;
} GraphicsManager;

GraphicsManager* GraphicsManager_Init(void);
void GraphicsManager_SetupGLAD(void);
void GraphicsManager_Clear(GraphicsManager* m);
void GraphicsManager_Render(GraphicsManager* m);
void GraphicsManager_Shutdown(GraphicsManager* m);

#endif