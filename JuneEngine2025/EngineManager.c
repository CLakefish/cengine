#include "Trace.h"
#include "Benchmark.h"
#include "EngineManager.h"

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
	Trace_Init(TR_INIT_PRINT);

	manager.graphicsManager = Graphics_Init();
	manager.serializer		= Serializer_Init("engineSerialized.bin", NULL);
	manager.timeManager		= Time_Init();
	manager.inputs			= Input_Init();

	Input_Load(manager.inputs);

	//Input_AddBind(manager.inputs, "escape", GLFW_KEY_ESCAPE,	 IT_KEY);
	//Input_AddBind(manager.inputs, "mouse",  GLFW_MOUSE_BUTTON_1, IT_MOUSE);
	//Input_AddBind(manager.inputs, "mousePos", -1, IT_MOUSEVEC);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		perror("Failed to initialize GLAD!");
		return;
	}

	Graphics_SetupGLAD();
	Engine_Run();
}

void Engine_Run(void) 
{
	while (!glfwWindowShouldClose(manager.graphicsManager->window)) {
		glfwPollEvents();
		Input_CalculateEvents(manager.graphicsManager->window, manager.inputs);

		if (Input_NamedInput(manager.inputs, "escape")->data.iBool.pressed) { 
			int disabled = glfwGetInputMode(manager.graphicsManager->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
			glfwSetInputMode(manager.graphicsManager->window, GLFW_CURSOR, disabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		}

		if (Input_NamedInput(manager.inputs, "mouse")->data.iBool.pressed) {
			printf("click!\n");
		}

		input_t* vec = Input_NamedInput(manager.inputs, "mousePos");
		printf("%f, %f\n", vec->data.iVec.currX, vec->data.iVec.currY);

		Time_Calculate(manager.timeManager);
		Graphics_Render(manager.graphicsManager);
	}

	Engine_Shutdown();
}

void Engine_Shutdown(void) 
{
	Input_Save(manager.inputs);

	Input_Shutdown(manager.inputs);
	Graphics_Shutdown(manager.graphicsManager);
	Time_Shutdown(manager.timeManager);

	Serializer_Destroy(manager.serializer);

	Trace_Shutdown();

	glfwTerminate();
}