#include "Trace.h"
#include "Benchmark.h"
#include "EngineManager.h"

#include "EntityManager.h"

#include "DynamicArray.h"

static EngineManager manager;

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
	Time_Init();
	Trace_Init(TR_INIT_PRINT);
	EntityManager_Init();

	manager.graphicsManager = Graphics_Init();
	manager.serializer = Serializer_Init("engineSerialized.bin", NULL);
	manager.inputs = Input_Init();

	Input_Load(manager.inputs);

	glfwSetInputMode(manager.graphicsManager->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		perror("Failed to initialize GLAD!");
		return;
	}

	Graphics_SetupGLAD();
	GizmoRenderer_Init();

	Engine_Run();
}

void Engine_Run(void)
{
	while (!glfwWindowShouldClose(manager.graphicsManager->window)) {

		Benchmarker b;
		Benchmark_Init();
		Benchmark_Start(&b);

		glfwPollEvents();
		Input_CalculateEvents(manager.graphicsManager->window, manager.inputs);

		if (Input_Bool(manager.inputs, "escape").pressed || Input_Bool(manager.inputs, "tab").pressed) {
			int disabled = glfwGetInputMode(manager.graphicsManager->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
			glfwSetInputMode(manager.graphicsManager->window, GLFW_CURSOR, disabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

			Input_NamedInput(manager.inputs, "mousePos")->data.iVec = (inputVec2_t){ 0,0,0,0 };
			Input_NamedInput(manager.inputs, "mousePos")->disabled = disabled;
		}

		Graphics_Render(manager.graphicsManager);

		Time_Calculate();
		glfwSwapBuffers(manager.graphicsManager->window);

		Benchmark_End(&b);
		printf("Time in ms for rendering: %f\n", Benchmark_Difference(&b) * 1000);
	}

	Engine_Shutdown();
}

void Engine_Shutdown(void)
{
	Input_Save(manager.inputs);

	Input_Shutdown(manager.inputs);
	EntityManager_Shutdown();
	Graphics_Shutdown(manager.graphicsManager);

	Serializer_Destroy(manager.serializer);

	Trace_Shutdown();
	glfwTerminate();
}