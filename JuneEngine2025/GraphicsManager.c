#include "Matrix4x4.h"
#include "GraphicsManager.h"
#include "Trace.h"

#include "EngineManager.h"
#include "InputManager.h"

float pitch, yaw = 90;
float moveSpeed = 10;

static void Graphics_CameraMovement(GraphicsManager* m) {
	EngineManager* engine = Engine_GetInstance();

	inputVec2_t i = Input_Vec(engine->inputs, "mousePos");
	float dX = (float)i.currX - (float)i.prevX;
	float dY = (float)i.currY - (float)i.prevY;

	float sensitivity = 0.1f;
	dX *= sensitivity;
	dY *= sensitivity;

	yaw   -= dX;
	pitch += dY;

	CLAMP(pitch, -89.9f, 89.9f);

	vec3_t dir = {
		cosf(yaw * DEG2RAD) * cosf(pitch * DEG2RAD),
		sinf(pitch * DEG2RAD),
		sinf(yaw * DEG2RAD) * cosf(pitch * DEG2RAD),
	};

	m->camera.forward = vec3_Normalize(dir);
	m->camera.right   = vec3_Normalize(vec3_Cross(m->camera.forward, (vec3_t) { 0, 1, 0 }));
	m->camera.up	  = vec3_Normalize(vec3_Cross(m->camera.right, m->camera.forward));

	if (Input_Bool(engine->inputs, "W").held) {
		vec3_t dir = vec3_Mult(m->camera.forward, moveSpeed * timeManager.deltaTime);
		m->camera.transform.position = vec3_Sub(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "S").held) {
		vec3_t dir = vec3_Mult(m->camera.forward, moveSpeed * timeManager.deltaTime);
		m->camera.transform.position = vec3_Add(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "A").held) {
		vec3_t dir = vec3_Mult(m->camera.right, moveSpeed * timeManager.deltaTime);
		m->camera.transform.position = vec3_Sub(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "D").held) {
		vec3_t dir = vec3_Mult(m->camera.right, moveSpeed * timeManager.deltaTime);
		m->camera.transform.position = vec3_Add(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "space").held) {
		vec3_t dir = vec3_Mult((vec3_t) { 0, 1, 0 }, moveSpeed* timeManager.deltaTime);
		m->camera.transform.position = vec3_Add(m->camera.transform.position, dir);
	}

	if (Input_Bool(engine->inputs, "leftShift").held) {
		vec3_t dir = vec3_Mult((vec3_t) { 0, 1, 0 }, moveSpeed* timeManager.deltaTime);
		m->camera.transform.position = vec3_Sub(m->camera.transform.position, dir);
	}
}

static void GraphicsManager_WindowCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GraphicsManager* Graphics_Init(void) {
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
	m->camera.transform.position	= (vec3_t){ 0, 1, 0 };
	m->camera.forward				= (vec3_t){ 0, 0, 1.0f };
	m->camera.up					= (vec3_t){ 0, 1, 0.0f };
	m->camera.right					= vec3_Normalize(vec3_Cross(m->camera.up, m->camera.forward));
	m->camera.FOV = 75;
	m->camera.nearPlane = 0.01f;
	m->camera.farPlane = 1000;

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

	int width, height;
	glfwGetWindowSize(m->window, &width, &height);
	float aspectRatio = (float)width / (float)height;

	mat4x4_t proj = mat_Perspective(m->camera.FOV * DEG2RAD, aspectRatio, m->camera.nearPlane, m->camera.farPlane);
	mat4x4_t view = mat_LookAt     (m->camera.transform.position, m->camera.forward, m->camera.right, m->camera.up);

	Graphics_CameraMovement(m);

	GizmoRenderer_Render(&view, &proj, m->camera.transform.position);
}

void Graphics_Shutdown(GraphicsManager* m) {
	glfwDestroyWindow(m->window);
	m->window = NULL;

	free(m);
}