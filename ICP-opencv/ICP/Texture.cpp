#include"Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	type = texType;

	cv::Mat img = cv::imread(image);

	cv::flip(img, img, 0);

	glGenTextures(1, &ID);

	glActiveTexture(slot);
	glBindTexture(texType, ID);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(texType, 0, GL_RGBA, img.cols,	img.rows, 0, format, pixelType,	img.ptr());
	glGenerateMipmap(texType);

	glBindTexture(texType, 0);
}

Texture::Texture(){}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}