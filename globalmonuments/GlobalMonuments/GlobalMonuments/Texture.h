#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/freeglut.h>
#include <GL/GL.h>
#include <string>

class Texture {
private:
	GLuint textureId;
	int width, height;
	int bpp;
	std::string filename;
public:
	Texture();
	Texture(std::string filename, GLuint textureId, int width = 256, int height = 256, int bpp = 4);
	void loadTexture();
	void activateTexture();
	int getWidth() const;
	int getHeight() const;
	std::string getFilename() const;
};

#endif