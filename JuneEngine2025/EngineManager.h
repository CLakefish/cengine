#pragma once

#ifndef GAME_MANAGER
#define GAME_MANAGER

#include <GLFW/glad.h>
#include <GLFW/glfw3.h>

#include "TimeManager.h"
#include "InputManager.h"
#include "GraphicsManager.h"

#include "Serializer.h"

typedef struct EngineManager {
	GraphicsManager* graphicsManager;
	TimeManager*	 timeManager;
	InputManager*	 inputs;

	Serializer*		 serializer;

	int	debugMode;
} EngineManager;

EngineManager* Engine_GetInstance	(void);
void		   Engine_Init			(void);
void		   Engine_Run			(void);
void		   Engine_Shutdown		(void);

#endif