#pragma once
#include "textUtil.h"
#include "model.h"
#include "resources.h"

void game(GLFWwindow* window, unsigned int shader, unsigned int texShader, unsigned int lightShader, GameState* gameState, Resources& resources, const char* texturePath);