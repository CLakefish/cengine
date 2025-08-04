#ifndef TIME_MANAGER
#define TIME_MANAGER

typedef struct TimeManager {
	float timeScale;

	float time;
	float deltaTime;
	float unscaledTime;
	float unscaledDeltaTime;

	float lastFrame;
} TimeManager;

extern TimeManager timeManager;

void Time_Init(void);
void TimeManager_Calculate(void);

#endif