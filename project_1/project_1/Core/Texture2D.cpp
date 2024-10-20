#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Texture2D.h"

Texture2D::Texture2D(const char* texturePath) {
	//Generate the texture object
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	//Wrapping behaviour parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Filtering parameters
	// options are:
	// GL_NEAREST_MIPMAP_NEAREST
	// GL_LINEAR_MIPMAP_NEAREST
	// GL_NEAREST_MIPMAP_LINEAR
	// GL_LINEAR_MIPMAP_LINEAR
	// but don't use MIPMAP settings for MAG_FILTER! (throws GL_INVALID_ENUM)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Loading the texture from the providen file path
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	//free the image memory
	stbi_image_free(data);
}
