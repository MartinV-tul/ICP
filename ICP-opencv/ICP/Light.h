#ifndef Light_H
#define Light_H

#include <iostream>
#include"Texture.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"OBJloader.h"
#include"Mesh.h"

class Light {
public:
	Light(Mesh modelMesh,glm::vec3 lightColor, GLuint shaderID);
	void draw(glm::mat4 vp);
	void translate(glm::vec3 moveVector);
	void rotate(float angle, glm::vec3 axis);
	void scale(glm::vec3 scaleValues);
	void resetModelMatrix();
	glm::vec4 getWorldPos();
	glm::mat4 modelMatrix;
	glm::vec3 color;
private:
	GLuint mvpID;
	GLuint colorID;
	Mesh mesh;
};
#endif