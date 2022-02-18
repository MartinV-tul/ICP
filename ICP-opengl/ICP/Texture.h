#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H
#include<GL/glew.h>
#include <opencv2\opencv.hpp>

class Texture
{
public:
	GLuint ID;
	GLenum type;
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
	Texture();

	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif