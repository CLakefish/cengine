#include "Trace.h"
#include "Benchmark.h"
#include "EngineManager.h"

static EngineManager manager;

EngineManager* Engine_GetInstance(void) {
	return &manager;
}

gizmo_t* gizmo3;

void Engine_Init(void) 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Benchmark_Init();
	Time_Init();
	Trace_Init(TR_INIT_PRINT);

	manager.graphicsManager = Graphics_Init();
	manager.serializer		= Serializer_Init("engineSerialized.bin", NULL);
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

	gizmo3 = Gizmo_CreateBox((vec3_t) { 0, 0, 0 }, (vec3_t) { 1, 1, 1 }, (vec3_t) { 1, 0, 1, 1 });
	GizmoRenderer_AddGizmo(gizmo3);

	Engine_Run();
}

void Engine_Run(void) 
{
	float total = 25;
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

		Benchmarker b;
		Benchmark_Init();
		Benchmark_Start(&b);

		Graphics_Render(manager.graphicsManager);

		Benchmark_End(&b);
		printf("Time in ms: %f. Total time: %f\n", Benchmark_Difference(&b) * 1000, timeManager.time);

		Time_Calculate();
		glfwSwapBuffers(manager.graphicsManager->window);
	}

	Engine_Shutdown();
}

void Engine_Shutdown(void) 
{
	Input_Save(manager.inputs);
	Input_Shutdown(manager.inputs);
	Graphics_Shutdown(manager.graphicsManager);
	GizmoRenderer_Shutdown();

	Serializer_Destroy(manager.serializer);

	Trace_Shutdown();
	glfwTerminate();
}