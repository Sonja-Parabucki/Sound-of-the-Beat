#pragma once
#include "lights.h"
#include "model.h"

class Lights {
public:
	Lights(unsigned int shader);
	void draw(glm::mat4 projectionView);
private:
	unsigned int shader;
	Model modelZ;	//along the z-axis
	Model modelX;	//along the x-axis
};