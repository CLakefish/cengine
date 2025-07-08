#ifndef INPUT
#define INPUT

#include <GLFW/glfw3.h>
#include <string.h>

#include "Serializer.h"
#include "stdinc.h"

typedef enum input_type {
	IT_KEY,
	IT_MOUSE,
	IT_MOUSEVEC,
	IT_JOYSTICK,
	IT_JOYSTICKVEC,
} input_type;

typedef struct inputVec2_t {
	double currX, currY;
	double prevX, prevY;
} inputVec2_t;

typedef struct inputBool_t {
	int pressed, held, released;
} inputBool_t;

typedef union input_data {
	inputVec2_t iVec;
	inputBool_t iBool;
} input_data;

typedef struct input_t {
	char* name;
	int keyCode;

	input_type type;
	input_data data;

	int disabled;
} input_t;

typedef struct InputManager {
	input_t* inputs;
	int count, capacity;

	Serializer* serializer;
} InputManager;

InputManager*	Input_Init				(void);
void			Input_Shutdown			(InputManager* m);
void			Input_Save				(InputManager* m);
void			Input_Load				(InputManager* m);
input_t*		Input_AddBind			(InputManager* m, char* name, int keyCode, input_type type);
void			Input_RemoveBind		(InputManager* m, int keyCode);
input_t*		Input_KeycodeInput		(InputManager* m, int keyCode);
input_t*		Input_NamedInput		(InputManager* m, const char* name);
void			Input_CalculateEvents	(GLFWwindow* w, InputManager* m);
void			Input_Check				(GLFWwindow* w, input_t* data);

#endif