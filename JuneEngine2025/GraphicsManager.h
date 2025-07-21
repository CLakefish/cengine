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

GraphicsManager* Graphics_Init(void);
void             Graphics_SetupGLAD(void);
void             Graphics_Clear(GraphicsManager* m);
void             Graphics_Render(GraphicsManager* m);
void             Graphics_Shutdown(GraphicsManager* m);

#endif