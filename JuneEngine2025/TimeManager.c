#pragma once
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include "TimeManager.h"
#include "Trace.h"

TimeManager* Time_Init(void) 
{
	TimeManager* t = (TimeManager*)malloc(sizeof(TimeManager));
	if (!t) {
		perror("Couldn't malloc for time manager!");
		return NULL;
	}

	*t = (TimeManager){ 1,0,0,0,0 };
	return t;
}

void Time_Shutdown(TimeManager* t) 
{
	free(t);
}

void Time_Calculate(TimeManager* timeManager) {
	float current = (float)glfwGetTime();

	timeManager->unscaledDeltaTime = (current - timeManager->lastFrame);
	timeManager->deltaTime		   = timeManager->unscaledDeltaTime * timeManager->timeScale;

	timeManager->unscaledTime += timeManager->unscaledDeltaTime;
	timeManager->time		  += timeManager->deltaTime;
	timeManager->lastFrame	   = current;
}