#pragma once
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "TimeManager.h"
#include "Trace.h"

TimeManager timeManager;

void Time_Init(void)
{
	timeManager = (TimeManager){ 1,0,0,0,0,0 };
}

void TimeManager_Calculate(void) {
	float current = (float)glfwGetTime();

	timeManager.unscaledDeltaTime = (current - timeManager.lastFrame);
	timeManager.deltaTime = timeManager.unscaledDeltaTime * timeManager.timeScale;

	timeManager.unscaledTime += timeManager.unscaledDeltaTime;
	timeManager.time += timeManager.deltaTime;
	timeManager.lastFrame = current;
}