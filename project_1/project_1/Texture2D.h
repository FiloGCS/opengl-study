#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "stb_image.h"

class Texture2D {
public:
	unsigned int ID;

	Texture2D(const char* texturePath);
	//void setClampMode();
	//void setFilteringMode();

private:

};

#endif