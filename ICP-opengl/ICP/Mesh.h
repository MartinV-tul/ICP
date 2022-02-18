#pragma once
#ifndef Mesh_H
#define Mesh_H

#include <iostream>
#include"Texture.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"OBJloader.h"

class Mesh {
public:
	Mesh(const char* objPath);
	Mesh();

	void draw();
	void changeUvsCoordinates(float dy);
	void deleteBuffers();
private:
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;

	GLuint VertexArrayID;

	GLuint uvsID;
	GLuint vertID;
	GLuint normID;

	GLsizei count;
};

#endif