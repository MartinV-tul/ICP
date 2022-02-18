#ifndef Ghost_H
#define Ghost_H

#include"Model.h"
#include"Mesh.h"

class Ghost {
public:
	Ghost(Mesh bodyMesh, Mesh eyeMesh, Mesh pupilMesh, Texture texture, GLuint shaderID);
	void rotate(glm::vec3 axis, float angle);
	void translate(glm::vec3 moveVector);
	void scale(glm::vec3 scaleValue);
	void assemble();
	void resetModelMatrix();
	void sortParts(glm::vec3 cameraPos);
	void draw(glm::mat4 vp, glm::vec3 cameraPos);
	void changeColor(float color);
	glm::vec3 getWorldPos();
	Model body;
	Model firstEye;
	Model secondEye;
	Model firstPupil;
	Model secondPupil;
	Model* sortedGhostParts[4];
	float distances[4];
	glm::vec3 position;
	glm::vec3 scaleValue;
	float angle;
	const float PINK = 0.0f;
	const float RED = 0.25f;
	const float GREEN = 0.5f;
	const float BLUE = 0.75f;
private:
	void translateParts();
	void rotateParts();
	void scaleParts();
	float color;
};
#endif