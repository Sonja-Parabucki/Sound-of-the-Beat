#pragma once
#include "textUtil.h"
#include "model.h"

int game(GLFWwindow* window, unsigned int shader, unsigned int texShader, unsigned int basicTexShader, unsigned int lightShader, GameState* gameState, const char* texturePath);