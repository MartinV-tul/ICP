#version 330 core

layout (location = 0) in vec3 FragPos;

uniform mat4 VP;
uniform vec3 cameraPos;

void main()
{
	gl_PointSize = min(20.0 / length(cameraPos - FragPos),5.0);
	gl_Position = VP * vec4(FragPos, 1.0f);
}