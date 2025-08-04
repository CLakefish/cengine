#include "Trace.h"
#include "Benchmark.h"
#include "EngineManager.h"

#include "EntityManager.h"

#include "DynamicArray.h"

static EngineManager manager;

EngineManager* Engine_GetInstance(void) {
	return &manager;
}

void Funcky(void) {
	printf("Hello!\n");
}

void Funcky2(void) {
	printf("Update!\n");
}

void Funcky123(void) {
	printf("Bye!\n");
}

entity_t* entity;

void Engine_Init(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Trace_Init(TR_INIT_PRINT);

	Benchmark_Init();
	Time_Init();

	manager.entityManager    = EntityManager_Init();
	manager.transformManager = TransformManager_Init();
	manager.graphicsManager  = GraphicsManager_Init();
	manager.behaviorManager  = BehaviorManager_Init();

	manager.serializer		 = Serializer_Init("engineSerialized.bin", NULL);
	manager.inputs			 = Input_Init();

	Input_Load(manager.inputs);
	glfwSetInputMode(manager.graphicsManager->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		perror("Failed to initialize GLAD!");
		return;
	}

	GraphicsManager_SetupGLAD();
	GizmoRenderer_Init();

	entity = Entity_Instantiate(manager.entityManager);
	transform_t* pos = AddTransform(manager.transformManager, (vec3_t) { 0 }, (vec3_t) { 0 }, (vec3_t) { 1 }, entity->ID);

	AddBehavior(manager.behaviorManager, "testing",  NULL, entity->ID, Funcky, Funcky2, Funcky123);
	AddBehavior(manager.behaviorManager, "testing2", NULL, entity->ID, Funcky, Funcky, Funcky123);
	AddBehavior(manager.behaviorManager, "testing3", NULL, entity->ID, Funcky, Funcky123, Funcky123);

	printf("%zu\n", manager.entityManager->dataCount);
	EntityManager_Debug(manager.entityManager);

	Engine_Run();
}

void Engine_Run(void)
{
	while (!glfwWindowShouldClose(manager.graphicsManager->window)) {
		glfwPollEvents();
		Input_CalculateEvents(manager.graphicsManager->window, manager.inputs);

		RemoveBehavior(manager.behaviorManager, entity->ID, "testing3");

		TransformManager_HandleDeletions(manager.transformManager);
		BehaviorManager_Update(manager.behaviorManager);

		if (Input_Bool(manager.inputs, "escape").pressed || Input_Bool(manager.inputs, "tab").pressed) {
			int disabled = glfwGetInputMode(manager.graphicsManager->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
			glfwSetInputMode(manager.graphicsManager->window, GLFW_CURSOR, disabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

			Input_NamedInput(manager.inputs, "mousePos")->data.iVec = (inputVec2_t){ 0 };
			Input_NamedInput(manager.inputs, "mousePos")->disabled = disabled;
		}

		Benchmarker b;
		Benchmark_Init();
		Benchmark_Start(&b);

		GraphicsManager_Render(manager.graphicsManager);

		Benchmark_End(&b);
		//printf("Time in ms for rendering: %f\n", Benchmark_Difference(&b) * 1000);
		
		TimeManager_Calculate();
		glfwSwapBuffers(manager.graphicsManager->window);
	}

	Engine_Shutdown();
}

void Engine_Shutdown(void)
{
	Input_Save(manager.inputs);
	Input_Shutdown(manager.inputs);

	BehaviorManager_Shutdown(manager.behaviorManager);
	GraphicsManager_Shutdown(manager.graphicsManager);

	TransformManager_Shutdown(manager.transformManager);
	EntityManager_Shutdown(manager.entityManager);

	Serializer_Destroy(manager.serializer);

	Trace_Shutdown();
	glfwTerminate();
}