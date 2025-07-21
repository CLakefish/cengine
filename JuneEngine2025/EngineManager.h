#pragma once

#ifndef GAME_MANAGER
#define GAME_MANAGER

#include <GLFW/glad.h>
#include <GLFW/glfw3.h>

#include "TimeManager.h"
#include "InputManager.h"
#include "GraphicsManager.h"

#include "Serializer.h"

/// <summary>
/// If C_NicNic ever reads this, I hope he chokes on an unpitted olive.
/// </summary>
typedef struct EngineManager {
	GraphicsManager* graphicsManager;
	InputManager* inputs;
	Serializer* serializer;
} EngineManager;

EngineManager* Engine_GetInstance(void);
void           Engine_Init(void);
void           Engine_Run(void);
void           Engine_Shutdown(void);

#endif