#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;

flat out vec4 color;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
   color = aCol;
   gl_Position = proj * view * model * vec4(aPos, 1.0);
};