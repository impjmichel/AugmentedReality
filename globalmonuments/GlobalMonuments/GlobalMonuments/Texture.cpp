#include <stb_image.c>
#include "Texture.h"

Texture::Texture() {}

Texture::Texture(std::string filename, GLuint textureId, int width, int height, int bpp) : filename(filename), textureId(textureId), width(width), height(height), bpp(bpp) {
}

Texture::Texture(unsigned char* imgData, GLuint textureId, int width, int height, int bpp) : imgData(imgData), textureId(textureId), width(width), height(height), bpp(bpp) {
}

void Texture::loadTexture() {

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	if (imgData == NULL) {
		this->imgData = stbi_load(filename.c_str(), &width, &height, &bpp, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->imgData);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, this->imgData);
	}
	
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, this->imgData);	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::activateTexture() {
	glBindTexture(GL_TEXTURE_2D, textureId);
}

int Texture::getWidth() const {
	return width;
}

int Texture::getHeight() const {
	return height;
}

std::string Texture::getFilename() const {
	return filename;
}