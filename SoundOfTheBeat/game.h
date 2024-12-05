#pragma once
#include "textUtil.h"

int game(GLFWwindow* window, unsigned int shader, unsigned int rayShader, unsigned int texShader, GameState& gameState, std::vector<double> beats, irrklang::ISound* song);