#ifndef SHADER
#define SHADER

#include <stdbool.h>

#include "Matrix4x4.h"

typedef struct shader_t {
	unsigned int ID;
} shader_t;

shader_t*	Shader_Init			(char* vertex, char* fragment);
void		Shader_Use			(shader_t* shader);
void		Shader_Clear		(shader_t* shader);
void		Shader_SetBool		(const shader_t* shader, const char* name, const bool value);
void		Shader_SetInt		(const shader_t* shader, const char* name, const int value);
void		Shader_SetFloat		(const shader_t* shader, const char* name, const float value);
void		Shader_SetMat4		(const shader_t* shader, const char* name, const mat4x4_t* value);

#endif // !SHADER
