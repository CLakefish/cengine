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

TimeManager* Time_Init		(void);
void		 Time_Calculate	(TimeManager* t);
void		 Time_Shutdown  (TimeManager* t);

#endif