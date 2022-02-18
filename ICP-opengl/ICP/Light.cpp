#include"Light.h"

Light::Light(Mesh modelMesh, glm::vec3 lightColor, GLuint shaderID) {
	mesh = modelMesh;
	color = lightColor;
	mvpID = glGetUniformLocation(shaderID, "MVP");
	colorID = glGetUniformLocation(shaderID, "lightColor");
	modelMatrix = glm::mat4(1.0f);
}

void Light::translate(glm::vec3 moveVector) {
	modelMatrix = glm::translate(modelMatrix, moveVector);
}

void Light::rotate(float angle, glm::vec3 axis) {
	modelMatrix = glm::rotate(modelMatrix, angle, axis);
}

void Light::scale(glm::vec3 scaleValues) {
	modelMatrix = glm::scale(modelMatrix, scaleValues);
}

void Light::draw(glm::mat4 vp) {
	glm::mat4 MVP = vp * modelMatrix;
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);
	glUniform4f(colorID, color.r, color.g, color.b, 1);

	mesh.draw();
}

void Light::resetModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
}

glm::vec4 Light::getWorldPos() {
	return modelMatrix * glm::vec4(0, 0, 0, 1);
}