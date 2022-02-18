#ifndef Model_H
#define Model_H

#include <iostream>
#include"Texture.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"OBJloader.h"
#include"Mesh.h"

class Model {
	public:
		Model(Mesh modelMesh, Texture modelTexture, GLuint shaderID);
		Model();
		void draw(glm::mat4 vp);
		void translate(glm::vec3 moveVector);
		void rotate(float angle, glm::vec3 axis);
		void scale(glm::vec3 scaleValues);
		void setTexture(Texture modelTexture);
		void resetModelMatrix();
		glm::vec4 getWorldPos();
		glm::mat4 modelMatrix;
		Mesh mesh;
	private:
		Texture texture;
		GLuint mvpID;
		GLuint modelMatrixID;
		time_t id;
};
#endif