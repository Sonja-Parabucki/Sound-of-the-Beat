#pragma once
#include "setup.h"

class Background {
public:
	Background(unsigned int shader);
	~Background();
	void draw(glm::mat4& projectionView);
	void setLogoTexture(const char* path);
private:
	unsigned int shader, VAO, VBO;
	unsigned int backgroundTexture, logoTexture;
};