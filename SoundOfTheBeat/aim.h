#pragma once
#include "setup.h"

class Aim {
public:
	Aim(unsigned int shader);
	~Aim();
	void draw();
private:
	unsigned int shader;
	unsigned int VAO, VBO;
};