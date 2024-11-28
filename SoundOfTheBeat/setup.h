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


unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);

unsigned int windowWidth();

unsigned int windowHeight();

struct Game {
	int mode;
	int next;
};

struct Ball {
    float x, y;
    //vreme kad da se pojavi i kad treba da se stisne
    //svaka svoju brzinu?
    bool hit;
    float inflation;
    bool red;
};

struct GameState {
    int score;
    int mode;
    double time;
    std::vector<Ball> balls;
};

