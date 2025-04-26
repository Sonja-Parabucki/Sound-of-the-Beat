#include "game.h"
#include <random>

#define r 0.12
#define BALL_GEN_DELTA 2.5
#define BOMB_GEN_DELTA BALL_GEN_DELTA - 0.5

GameState* state;
int combo = 1;
int wWidth, wHeight;
float aspectRatio;

float lastX, lastY;
float yaw, pitch;
bool firstFrame;
bool wasSpacePressed;

bool won;
bool gameOver;
glm::vec3 explosionPos;
float explosionInflation;
bool paused;
bool endGame;

glm::vec3 cameraAt;
glm::vec3 cameraFront;
glm::vec3 cameraUp;

glm::mat4 projectionView;
glm::mat4 view;
glm::mat4 projection;
std::string messageTx;


float random() {
    return -GEN_LIMIT + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (GEN_LIMIT *2)));
}

void setCombo() {
    combo = 1;
    if (state->streak >= 8) combo = 8;
    else if (state->streak >= 4) combo = 4;
    else if (state->streak >= 2) combo = 2;
}

void checkWin() {
    if (state->lastBeat == state->beatTimes.size() && !gameOver) {
        for (const Ball& ball : state->balls)
            if (!ball.hit) return;
        if (!won) {
            playWin();
            messageTx = "CONGRATULATIONS";
        }
        won = true;
    }
}

void setGameOver(bool isExplosion = false) {
    state->score = 0;
    pauseSong(state->song);
    gameOver = true;
    messageTx = "GAME OVER";
    if (isExplosion)
        playExplosion();
    else
        playGameOver();
}

void generateBall(int beatInd) {
    if (beatInd >= state->beatTimes.size()) return;
    Ball ball{ glm::vec3(random(), random(), 0.f), state->beatTimes.at(beatInd), false, 1};
    if (state->mode == 1) ball.red = ball.pos[0] <= 0;
    state->balls.push_back(ball);
}

void generateBomb() {
    Bomb bomb{ glm::vec3(random(), random(), 0.f) };
    state->bombs.push_back(bomb);
}

void updateBalls() {
    for (auto it = state->balls.begin(); it != state->balls.end();) {
        it->pos[2] += SPEED;
        
        if (it->pos[2] >= Z_LIMIT * 0.95 && !it->hit) {   // => miss
            it->hit = true;
            state->score -= 5;
            combo = 1;
            state->streak = 0;
            playRay();

            if (state->score <= 0) setGameOver();
        }
        if (it->hit) it->inflation *= INFLATION_SPEED;

        if (it->pos[2] > Z_LIMIT || it->inflation < 0.01)   //not visible
            it = state->balls.erase(it);
        else ++it;
    }
    checkWin();
}

void updateBombs() {
    for (auto it = state->bombs.begin(); it != state->bombs.end();) {
        it->pos[2] += BOMB_SPEED;
        if (it->pos[2] > Z_LIMIT)   //fell off from the screen
            it = state->bombs.erase(it);
        else ++it; 
    }
}


glm::vec3 clickToWorldCoord(GLFWwindow* window) {
    glm::vec4 rayClip = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
    rayWorld = glm::normalize(rayWorld);
    return rayWorld;
}

bool isTouching(glm::vec3 objPos, glm::vec3 rayWorld) {
    glm::vec3 oc = cameraAt - objPos;
    float a = glm::dot(rayWorld, rayWorld);
    float b = 2.0f * glm::dot(oc, rayWorld);
    float c = glm::dot(oc, oc) - r * r;
    float discriminant = b * b - 4 * a * c;
    return discriminant >= 0;
}

void checkBombs(glm::vec3 rayWorld) {
    for (const Bomb& bomb : state->bombs) {
        if (isTouching(bomb.pos, rayWorld)) {
            explosionPos = bomb.pos;
            explosionInflation = 0.01;
            setGameOver(true);

            for (Ball& ball : state->balls)
                ball.hit = true;
            state->bombs.clear();
            return;
        }
    }
}

bool checkShot(glm::vec3 rayWorld, bool leftClick) {
    for (auto it = state->balls.begin(); it != state->balls.end(); ++it) {
        if (it->hit) continue;
        
        if (state->mode == 1 && it->red != leftClick)
            continue;

        if (isTouching(it->pos, rayWorld)) {
            double t = glfwGetTime();
            if (abs(t - it->timeToHit) < 0.5) {
                state->streak += 1;
                setCombo();
                state->score += 2 * combo;
                playEffect();
            }
            else {
                //the hit is early or late => lose the streak, no points gained or lost
                state->streak = 0;
                setCombo();
                playMiss();
            }
            it->hit = true;
            checkWin();
            return true;
        }
    }
    return false;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (paused) return;
    if (!gameOver && (button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_LEFT) && action == GLFW_PRESS) {
        auto ray = clickToWorldCoord(window);
        if (checkShot(ray, button == GLFW_MOUSE_BUTTON_LEFT)) return;
        checkBombs(ray);
    }
}

void generateNewObjects() {
    double t = glfwGetTime();
    if ((state->beatTimes.size() > state->lastBeat) && (state->beatTimes.at(state->lastBeat) - t < BALL_GEN_DELTA)) {
        generateBall(state->lastBeat);
        state->lastBeat++;
    }

    if ((state->lastBomb < state->bombTimes.size()) && (state->bombTimes.at(state->lastBomb) - t < BOMB_GEN_DELTA)) {
        generateBomb();
        state->lastBomb++;
    }
}


void updateProjectionAndView() {
    glm::vec3 direction = glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
    cameraFront = glm::normalize(direction);

    view = glm::lookAt(cameraAt, cameraAt + cameraFront, cameraUp);

    projection = glm::perspective(glm::radians(15.0f), aspectRatio, 0.1f, 100.0f);

    projectionView = projection * view;
}

void processInput(GLFWwindow* window) {
    if (paused && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        endGame = true;
        return;
    }
    bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spacePressed && !wasSpacePressed) {
        if (paused) glfwSetTime(state->time);
        else {
            if (won || gameOver) endGame = true;
            else {
                pauseSong(state->song);
                state->time = glfwGetTime();
                firstFrame = true;
            }
        }
        paused = !paused;
        wasSpacePressed = true;
        return;
    }
    wasSpacePressed = spacePressed;
    if (paused) return;
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraAt += cameraSpeed * cameraUp;
        cameraAt.y = std::min(cameraAt.y, 0.8f);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraAt -= cameraSpeed * cameraUp;
        cameraAt.y = std::max(cameraAt.y, -0.6f);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraAt -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        cameraAt.x = std::max(cameraAt.x, -0.6f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraAt += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        cameraAt.x = std::min(cameraAt.x, 0.6f);
    }
    updateProjectionAndView();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (paused) return;
    if (firstFrame) {
        lastX = xpos;
        lastY = ypos;
        firstFrame = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.02f;
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if (yaw > -30.0f)
        yaw = -30.0f;
    if (yaw < -150.0f)
        yaw = -150.0f;

    if (pitch > 60.0f)
        pitch = 60.0f;
    if (pitch < -60.0f)
        pitch = -60.0f;
    updateProjectionAndView();
}

void drawPause(GLFWwindow* window) {
    double renderStart = glfwGetTime();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.05, 0.0, 0.0, 1.0);
    renderText("GAME PAUSED", 100, wHeight - 200, 2, 0.93, 0.94, 0.78);
    renderText("[SPACE] to CONTINUE", 120, wHeight / 2, 1, 1., 1., 1.);
    renderText("[ESC] back to MENU", 120, wHeight / 2 - 80, 1, 1., 1., 1.);
    std::string scoreTx = "SCORE: " + std::to_string(state->score);
    renderText(scoreTx, 120, 150, 1.2, 1., 1., 1.);

    limitFPS(renderStart);
}


void game(GLFWwindow* window, GameState* gameState, Resources& resources, const char* texturePath) {

    glfwGetFramebufferSize(window, &wWidth, &wHeight);
    aspectRatio = (float)wWidth / wHeight;
    //camera position
    cameraUp = glm::vec3(0.0f, 1.0f, 0.f);
    cameraAt = gameState->camera.pos;
    pitch = gameState->camera.pitch;
    yaw = gameState->camera.yaw;
    updateProjectionAndView();

    resources.background.setLogoTexture(texturePath);

    //options
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    firstFrame = true;
    wasSpacePressed = false;

    //setting the game state
    state = gameState;
    glfwSetTime(gameState->time);
    setCombo();
    endGame = false;
    won = false;
    gameOver = false;
    paused = false;
    explosionPos = glm::vec3(-1.0);

    std::string scoreTx;
    float scoreScaling = 1.0f;
    std::string comboTx;
    std::string continueTx = "press [SPACE]";

    double renderStart;
    while (!endGame) {
        if (glfwWindowShouldClose(window)) break;
        processInput(window);

        if (paused) drawPause(window);
        else {
            renderStart = glfwGetTime();
            state->song->setIsPaused(gameOver);
            
            if (!gameOver)
                generateNewObjects();
            if (state->score > 0 || explosionPos[0] > -1) {
                updateBalls();
                updateBombs();
            }
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            resources.background.draw(projectionView);
            resources.aim.draw();
            resources.lights.draw(projectionView);
            resources.ballModel.draw(state->balls, cameraAt, projectionView, state->mode);
            resources.bombModel.draw(state->bombs, cameraAt, projectionView);
            if (explosionPos[0] > -1) {
                if (explosionInflation < 1.0)
                    explosionInflation += EXPLOSION_SPEED;
                resources.explosion.draw(explosionPos, explosionInflation);
            }

            if (won || gameOver) {
                renderText(messageTx, 100, wHeight - 200, 2, 1, 1, 1);
                renderText(continueTx, 100, wHeight - 300, 1, 1, 1, 1);
                scoreScaling = 1.5f;
            }
            scoreTx = "SCORE: " + std::to_string(state->score);
            renderText(scoreTx, 20, 50, scoreScaling, 1.0, 1.0, 1.0);
            comboTx = 'x' + std::to_string(combo);
            renderText(comboTx, 20, 10, 0.8, 1.0, 0.5, 0.);

            limitFPS(renderStart);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();   
    }
}