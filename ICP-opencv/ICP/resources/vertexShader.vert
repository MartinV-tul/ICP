#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec2 UV;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 MVP;
uniform mat4 model;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	FragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0f));
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
	Normal = mat3(transpose(inverse(model))) * vertexNormal;
}