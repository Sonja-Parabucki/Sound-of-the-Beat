#pragma once
#include "setup.h"
#include "model.h"

class BombModel {
public:
	BombModel(unsigned int shader);
	void draw(const std::vector<Bomb>& bombs, glm::vec3 cameraAt, glm::mat4 projectionView);
private:
	unsigned int shader;
	Model model;
};