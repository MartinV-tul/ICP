#include "Model.h"

Model::Model(Mesh modelMesh, Texture modelTexture, GLuint shaderID) {
	mesh = modelMesh;
	texture = modelTexture;
	mvpID = glGetUniformLocation(shaderID, "MVP");
	modelMatrixID = glGetUniformLocation(shaderID, "model");
	modelMatrix = glm::mat4(1.0f);
	std::time(&id);
}

Model::Model(){}

void Model::translate(glm::vec3 moveVector) {
	modelMatrix = glm::translate(modelMatrix, moveVector);
}

void Model::rotate(float angle, glm::vec3 axis) {
	modelMatrix = glm::rotate(modelMatrix, angle, axis);
}

void Model::scale(glm::vec3 scaleValues) {
	modelMatrix = glm::scale(modelMatrix, scaleValues);
}

void Model::draw(glm::mat4 vp) {
	glm::mat4 MVP = vp * modelMatrix;
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
	
	texture.Bind();

	mesh.draw();
}

void Model::resetModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
}

void Model::setTexture(Texture modelTexture) {
	texture = modelTexture;
}

glm::vec4 Model::getWorldPos() {
	return modelMatrix * glm::vec4(0, 0, 0, 1);
}
