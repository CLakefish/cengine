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
	vec4 pos      = proj * vec4(aPos,1.0);
	float dist    = distance(vec3(0, 0, 0), vec3(pos.x, 0, pos.z));
	float lerpVal = mix(alpha, 0, dist / fogDist);
	return lerpVal;
};

void main(void)
{
   gl_Position = proj * view * model * vec4(aPos, 1.0);
   color = vec4(aCol.x, aCol.y, aCol.z, GetFogAlpha(aCol.w));
};