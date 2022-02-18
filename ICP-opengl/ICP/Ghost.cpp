#include"Ghost.h"

Ghost::Ghost(Mesh bodyMesh, Mesh eyeMesh, Mesh pupilMesh, Texture texture, GLuint shaderID) {
	Model ghostBody(bodyMesh, texture, shaderID);
	Model ghostFirstEye(eyeMesh, texture, shaderID);
	Model ghostSecondEye(eyeMesh, texture, shaderID);
	Model ghostFirstPupil(pupilMesh, texture, shaderID);
	Model ghostSecondPupil(pupilMesh, texture, shaderID);

	body = ghostBody;
	firstEye = ghostFirstEye;
	secondEye = ghostSecondEye;
	firstPupil = ghostFirstPupil;
	secondPupil = ghostSecondPupil;

	sortedGhostParts[0] = &firstEye;
	sortedGhostParts[1] = &secondEye;
	sortedGhostParts[2] = &firstPupil;
	sortedGhostParts[3] = &secondPupil;

	scaleValue = glm::vec3(1, 1, 1);
	color = 0.0f;
}

void Ghost::rotate(glm::vec3 axis, float ang) {
	angle += ang;
}

void Ghost::translate(glm::vec3 moveVector) {
	position += moveVector;
}

void Ghost::assemble() {
	firstEye.translate(glm::vec3(0.35f, 0.2f, -0.9f));
	secondEye.translate(glm::vec3(-0.35f, 0.2f, -0.9f));
	firstPupil.translate(glm::vec3(0.35f, 0.2f, -1.05f));
	secondPupil.translate(glm::vec3(-0.35f, 0.2f, -1.05f));
}

void Ghost::resetModelMatrix() {
	body.resetModelMatrix();
	firstEye.resetModelMatrix();
	secondEye.resetModelMatrix();
	firstPupil.resetModelMatrix();
	secondPupil.resetModelMatrix();
}

void Ghost::scale(glm::vec3 scaleV) {
	scaleValue = scaleV;
}

void Ghost::scaleParts() {
	body.scale(scaleValue);
	firstEye.scale(scaleValue);
	secondEye.scale(scaleValue);
	firstPupil.scale(scaleValue);
	secondPupil.scale(scaleValue);
}

void Ghost::translateParts() {
	body.translate(position);
	firstEye.translate(position);
	secondEye.translate(position);
	firstPupil.translate(position);
	secondPupil.translate(position);
}

void Ghost::rotateParts() {
	glm::vec3 axis(0, 1, 0);
	body.rotate(angle, axis);
	firstEye.rotate(angle, axis);
	secondEye.rotate(angle, axis);
	firstPupil.rotate(angle, axis);
	secondPupil.rotate(angle, axis);
}

glm::vec3 Ghost::getWorldPos() {
	return position;
}

void Ghost::sortParts(glm::vec3 cameraPos) {
	for (int i = 0; i < 4; i++)
	{
		distances[i] = glm::length(glm::vec4(cameraPos, 1) - sortedGhostParts[i]->getWorldPos());
	}

	int j;
	for (int i = 1; i < 4; i++)
	{
		float d = distances[i];
		Model* m = sortedGhostParts[i];
		j = i - 1;

		while (j > -1 && d > distances[j]) {
			distances[j + 1] = distances[j];
			sortedGhostParts[j + 1] = sortedGhostParts[j];
			j--;
		}
		distances[j + 1] = d;
		sortedGhostParts[j + 1] = m;
	}
}

void Ghost::draw(glm::mat4 vp, glm::vec3 cameraPos) {
	resetModelMatrix();
	translateParts();
	rotateParts();
	scaleParts();
	assemble();
	sortParts(cameraPos);
	for (int i = 0; i < 4; i++)
	{
		sortedGhostParts[i]->draw(vp);
	}
	body.draw(vp);
}

void Ghost::changeColor(float ghostColor) {
	body.mesh.changeUvsCoordinates(ghostColor - color);
	color = ghostColor;
}