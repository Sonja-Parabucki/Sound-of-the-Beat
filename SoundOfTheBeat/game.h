#pragma once
#include "textUtil.h"
#include "model.h"

int game(GLFWwindow* window, unsigned int shader, unsigned int rayShader, unsigned int texShader, unsigned int lightShader, GameState* gameState, const char* texturePath);