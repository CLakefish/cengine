#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in mat4 instanceMatrix;

flat out vec4 color;

uniform mat4 view;
uniform mat4 proj;

void main(void)
{
   gl_Position = proj * view * instanceMatrix * vec4(aPos, 1.0);
   color = aCol;
};