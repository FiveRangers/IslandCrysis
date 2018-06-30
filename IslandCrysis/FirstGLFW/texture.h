#pragma once

#include <glad/glad.h>
#include <iostream>
#include <string>
#include "stb_image.h"

using namespace std;

class Texture2D {
public:
	GLuint textureID;

	Texture2D();

	~Texture2D();

	void loadTexture(string path, bool constrain = false);
};