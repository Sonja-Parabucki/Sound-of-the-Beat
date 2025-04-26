#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "stb.h"
#include "song.h"

const double FPS = 60.0;
const double FRAME_TIME = 1.0 / FPS;

const float Z_LIMIT = 32.0f;

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned int loadImageToTexture(const char* filePath);

void initVABO(const float* vertices, size_t verticesLength, unsigned int stride, unsigned int* VAO, unsigned int* VBO, bool staticDraw);

void limitFPS(double renderStart);

unsigned int windowWidth();

unsigned int windowHeight();

struct Game {
	int mode;
	int next;
    std::string selectedSongName;
};

struct Ball {
    glm::vec3 pos;
    double timeToHit;
    bool hit;
    float inflation;
    bool red;
};

struct Bomb {
    glm::vec3 pos;
};

struct CameraPosition {
    glm::vec3 pos;
    float yaw;
    float pitch;
};

struct GameState {
    irrklang::ISound* song;
    int score;
    int streak;
    int mode;
    double time;
    std::vector<Ball> balls;
    std::vector<Bomb> bombs;
    std::vector<double> beatTimes;
    int lastBeat;
    std::vector<double> bombTimes;
    int lastBomb;
    CameraPosition camera;
};
