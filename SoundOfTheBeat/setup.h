#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "stb.h"
#include "song.h"

const double FPS = 60.0;
const double FRAME_TIME = 1.0 / FPS;


unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned int loadImageToTexture(const char* filePath);

void initVABO(const float* vertices, size_t verticesLength, unsigned int stride, unsigned int* VAO, unsigned int* VBO, bool staticDraw);


unsigned int windowWidth();

unsigned int windowHeight();

struct Game {
	int mode;
	int next;
    std::string selectedSongName;
};

struct Ball {
    float x, y;
    double timeToHit;
    bool hit;
    float inflation;
    bool red;
};

struct GameState {
    int score;
    int streak;
    int mode;
    double time;
    std::vector<Ball> balls;
    int lastBeat;
};

