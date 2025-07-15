#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;

flat out vec4 color;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;
uniform vec3 camPos;

uniform float fogDist;

float GetFogAlpha(float alpha)
{
	float dist = distance(vec4(camPos.x, 0, camPos.z, 1), gl_Position);
	float lerpVal = mix(alpha, 0, dist / fogDist);
	return lerpVal;
};

void main()
{
   gl_Position = proj * view * model * vec4(aPos, 1.0);
   color = new vec4(aCol.x, aCol.y, aCol.z, GetFogAlpha(aCol.w));
};