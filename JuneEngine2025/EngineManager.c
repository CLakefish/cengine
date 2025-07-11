#include "Trace.h"
#include "Benchmark.h"
#include "EngineManager.h"

static EngineManager manager;

gizmo_t* gizmo;
gizmo_t* gizmo2;

EngineManager* Engine_GetInstance(void) {
	return &manager;
}

void Engine_Init(void) 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Benchmark_Init();
	Trace_Init(TR_INIT_PRINT);

	manager.graphicsManager = Graphics_Init();
	manager.serializer		= Serializer_Init("engineSerialized.bin", NULL);
	manager.timeManager		= Time_Init();
	manager.inputs			= Input_Init();

	Input_AddBind(manager.inputs, "mouse",		GLFW_MOUSE_BUTTON_1,	IT_MOUSE);
	Input_AddBind(manager.inputs, "mousePos",	-1,						IT_MOUSEVEC);
	Input_AddBind(manager.inputs, "escape",		GLFW_KEY_ESCAPE,		IT_KEY);
	Input_AddBind(manager.inputs, "W",			GLFW_KEY_W,				IT_KEY);
	Input_AddBind(manager.inputs, "A",			GLFW_KEY_A,				IT_KEY);
	Input_AddBind(manager.inputs, "S",			GLFW_KEY_S,				IT_KEY);
	Input_AddBind(manager.inputs, "D",			GLFW_KEY_D,				IT_KEY);
	Input_AddBind(manager.inputs, "space",		GLFW_KEY_SPACE,			IT_KEY);
	Input_AddBind(manager.inputs, "leftShift",	GLFW_KEY_LEFT_SHIFT,	IT_KEY);
	Input_AddBind(manager.inputs, "tab",		GLFW_KEY_TAB,			IT_KEY);

	Input_Load(manager.inputs);

	glfwSetInputMode(manager.graphicsManager->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		perror("Failed to initialize GLAD!");
		return;
	}

	Graphics_SetupGLAD();

	GizmoRenderer_Init();

	gizmo = Gizmo_CreateLine((vec3_t) { 0, 0, 0 }, (vec3_t) {0,10,10}, (vec3_t) {0,1,1});
	GizmoRenderer_AddGizmo(gizmo);
	gizmo2 = Gizmo_CreateBox((vec3_t) { 0, 0, 0 }, (vec3_t) { 10, 10, 10 }, (vec3_t) { 1, 0, 1 });
	GizmoRenderer_AddGizmo(gizmo2);

	Engine_Run();
}

void Engine_Run(void) 
{
	int total = 50;
	for (float i = -total; i <= total; ++i) {
		for (float j = -total; j <= total; ++j) {
			GizmoRenderer_AddGizmo(Gizmo_CreateBox((vec3_t) { i + 45, (i * i) / (j * j), j }, (vec3_t) { 1, 1, 1 }, (vec3_t) { 1, 1, 1, 0.1f }));
		}
	}

	while (!glfwWindowShouldClose(manager.graphicsManager->window)) {
		glfwPollEvents();
		Input_CalculateEvents(manager.graphicsManager->window, manager.inputs);

		if (Input_Bool(manager.inputs, "escape").pressed || Input_Bool(manager.inputs, "tab").pressed) {
			int disabled = glfwGetInputMode(manager.graphicsManager->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
			glfwSetInputMode(manager.graphicsManager->window, GLFW_CURSOR, disabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

			Input_NamedInput(manager.inputs, "mousePos")->data.iVec = (inputVec2_t){ 0,0,0,0 };
			Input_NamedInput(manager.inputs, "mousePos")->disabled = disabled;
		}

		gizmo2->transform.position = (vec3_t){ 0, sinf(manager.timeManager->time), 0 };
		gizmo2->transform.rotation = (vec3_t){ 0, gizmo2->transform.rotation.y + manager.timeManager->deltaTime, 0 };

		Time_Calculate(manager.timeManager);
		Graphics_Render(manager.graphicsManager);

		glfwSwapBuffers(manager.graphicsManager->window);
	}

	Engine_Shutdown();
}

void Engine_Shutdown(void) 
{
	Input_Save(manager.inputs);

	Input_Shutdown(manager.inputs);
	Time_Shutdown(manager.timeManager);

	Graphics_Shutdown(manager.graphicsManager);
	GizmoRenderer_Shutdown();

	Serializer_Destroy(manager.serializer);

	Trace_Shutdown();

	glfwTerminate();
}