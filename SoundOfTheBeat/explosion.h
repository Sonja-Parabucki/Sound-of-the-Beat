#pragma once
#include "setup.h"

class Explosion {
public:
	Explosion(unsigned int shader);
	~Explosion();
	void draw(glm::vec3 explosionPos, float explosionInflation);
private:
	unsigned int VAO, VBO, shader;
	unsigned int texture;
};