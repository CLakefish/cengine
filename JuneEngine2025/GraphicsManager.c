#include "Matrix4x4.h"
#include "GraphicsManager.h"
#include "Trace.h"

#include "EngineManager.h"
#include "InputManager.h"

static float pitch = 0.0f;
static float yaw   = -90.0f;
static float moveSpeed = 25;

static void Graphics_CameraMovement(GraphicsManager* m) {
	EngineManager* engine = Engine_GetInstance();

	inputVec2_t i = Input_Vec(engine->inputs, "mousePos");
	float dX = (float)i.currX - (float)i.prevX;
	float dY = (float)i.currY - (float)i.prevY;

	float sensitivity = 0.1f;
	dX *= sensitivity;
	dY *= sensitivity;

	yaw   += dX;
	pitch += dY;

	CLAMP(pitch, -89.9f, 89.9f);

	// incorrect eulerAngles
	m->camera.transform.rotation = (vec3_t){ pitch, yaw, 0 };

	vec3_t forward = Transform_GetForward(&m->camera.transform);
	vec3_t right   = Transform_GetRight(&m->camera.transform);
	vec3_t up	   = Transform_GetUp(&m->camera.transform);

	if (Input_Bool(engine->inputs, "W").held) {
		vec3_t dir = vec3_Mult(forward, moveSpeed * timeManager.deltaTime);
		m->camera.transform.position = vec3_Sub(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "S").held) {
		vec3_t dir = vec3_Mult(forward, moveSpeed * timeManager.deltaTime);
		m->camera.transform.position = vec3_Add(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "A").held) {
		vec3_t dir = vec3_Mult(right, moveSpeed * timeManager.deltaTime);
		m->camera.transform.position = vec3_Sub(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "D").held) {
		vec3_t dir = vec3_Mult(right, moveSpeed * timeManager.deltaTime);
		m->camera.transform.position = vec3_Add(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "space").held) {
		vec3_t dir = vec3_Mult(up, moveSpeed* timeManager.deltaTime);
		m->camera.transform.position = vec3_Add(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "leftShift").held) {
		vec3_t dir = vec3_Mult(up, moveSpeed* timeManager.deltaTime);
		m->camera.transform.position = vec3_Sub(m->camera.transform.position, dir);
	}
}

static void GraphicsManager_WindowCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GraphicsManager* GraphicsManager_Init(void) {
	GraphicsManager* m = (GraphicsManager*)malloc(sizeof(GraphicsManager));

	if (!m) {
		perror("Unable to initialize graphics manager through malloc call!");
		return NULL;
	}

	GLFWwindow* wind = glfwCreateWindow(1080, 800, "test", NULL, NULL);
	if (!wind) {
		perror("Unable to initialize graphics manager window!");
		return NULL;
	}

	m->window = wind;

	glfwMakeContextCurrent(m->window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(m->window, GraphicsManager_WindowCallback);
	m->camera.transform.position = (vec3_t){ 0, 1, 0 };
	m->camera.FOV		= 75;
	m->camera.nearPlane = 0.01f;
	m->camera.farPlane = 1000;

	return m;
}

void GraphicsManager_SetupGLAD(void) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glCullFace(GL_FRONT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GraphicsManager_Clear(GraphicsManager* m) {
	glClearColor(0.1f, 0, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsManager_Render(GraphicsManager* m) {
	GraphicsManager_Clear(m);

	int width, height;
	glfwGetWindowSize(m->window, &width, &height);
	float aspectRatio = (float)width / (float)height;
	
	vec3_t forward	= Transform_GetForward(&m->camera.transform);
	vec3_t right	= Transform_GetRight(&m->camera.transform);
	vec3_t up		= Transform_GetUp(&m->camera.transform);

	mat4x4_t proj	= mat_Perspective(m->camera.FOV, aspectRatio, m->camera.nearPlane, m->camera.farPlane);
	mat4x4_t view	= mat_LookAt(m->camera.transform.position, forward, right, up);

	Graphics_CameraMovement(m);

	GizmoRenderer_Render(&view, &proj, m->camera.transform.position);
}

void GraphicsManager_Shutdown(GraphicsManager* m) {
	glfwDestroyWindow(m->window);
	m->window = NULL;

	free(m);

	GizmoRenderer_Shutdown();
}