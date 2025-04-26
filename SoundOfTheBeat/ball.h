#pragma once
#include "setup.h"
#include "model.h"

class BallModel {
public:
	BallModel(unsigned int shader);
	~BallModel();
	void draw(const std::vector<Ball>& balls, glm::vec3 cameraAt, glm::mat4 projectionView, int mode);
private:
	unsigned int shader;
	Model model;
};