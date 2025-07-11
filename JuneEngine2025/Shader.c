#include "Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

shader_t* Shader_Init(char* vertex, char* fragment)
{
	GLuint success;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); 
	glShaderSource(vertexShader, 1, &vertex, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("VERTEX SHADER FAIL: %s", infoLog);
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("FRAGMENT SHADER FAIL: %s", infoLog);
	}

	free(vertex);
	free(fragment);

	shader_t* shader = (shader_t*)malloc(sizeof(shader_t));

	if (!shader)
	{
		perror("Unable to initialize shader!");
		return NULL;
	}

	shader->ID = glCreateProgram();
	glAttachShader(shader->ID, vertexShader);
	glAttachShader(shader->ID, fragmentShader);
	glLinkProgram(shader->ID);

	glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shader->ID, 512, NULL, infoLog);
		printf("%s\n", infoLog);
		return NULL;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shader;
}

void Shader_Use(shader_t* shader)
{
	glUseProgram(shader->ID);
}

void Shader_Clear(shader_t* shader)
{
	glDeleteProgram(shader->ID);
	free(shader);
}

void Shader_SetBool(const shader_t* shader, const char* name, const bool value)
{
	glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}

void Shader_SetInt(const shader_t* shader, const char* name, const int value)
{
	glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void Shader_SetFloat(const shader_t* shader, const char* name, const float value)
{
	glUniform1f(glGetUniformLocation(shader->ID, name), value);
}

void Shader_SetMat4(const shader_t* shader, const char* name, const mat4x4_t* value)
{
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_TRUE, (GLfloat*)value);
}