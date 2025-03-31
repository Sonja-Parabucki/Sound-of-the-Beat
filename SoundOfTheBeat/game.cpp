#include "game.h"
#include <random>

#define SPEED 0.1
#define BOMB_SPEED SPEED + 0.05
#define INFLATION_SPEED 0.6
#define r 0.08
#define LIMIT 0.75
#define GEN_LIMIT 0.35
#define Z_LIMIT 32.0
#define DEATH_RAY_Y -LIMIT + 0.05
#define DEATH_RAY_FRAMES 6

GameState* state;
int combo = 1;
int wWidth, wHeight;

bool won;
bool gameOver;
glm::vec3 explosionPos;

glm::vec3 lightPosition = glm::vec3(LIMIT, LIMIT, 0.0f);

glm::vec3 cameraAt = glm::vec3(0.0f, 0.5f, Z_LIMIT);
glm::mat4 projectionView;
glm::mat4 view;
glm::mat4 viewInverse;
glm::mat4 projection;
glm::mat4 projectionInverse;
glm::mat4 model;
std::string messageTx;


float random() {
    return -GEN_LIMIT + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (GEN_LIMIT *2)));
}

float random(float a, float b) {
    return a + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (b-a)));
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

void setGameOver() {
    pauseSong(state->song);
    gameOver = true;
    messageTx = "GAME OVER";
    playGameOver();
}

void generateBall(int beatInd) {
    if (beatInd >= state->beatTimes.size()) {
        return;
    }
    Ball ball{ glm::vec3(random(), random(), 0.f), state->beatTimes.at(beatInd), false, 1};

    if (state->mode == 1)
        ball.red = ball.pos[0] <= 0;
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

            if (state->score <= 0)
                setGameOver();
        }

        if (it->hit)
            it->inflation *= INFLATION_SPEED;

        if (it->pos[2] > Z_LIMIT || it->inflation < 0.01)   //not visible
            it = state->balls.erase(it);
        else
            ++it;
    }
    checkWin();
}

void updateBombs() {
    for (auto it = state->bombs.begin(); it != state->bombs.end();) {
        it->pos[2] += BOMB_SPEED;
        if (it->pos[2] > Z_LIMIT)   //fell off from the screen
            it = state->bombs.erase(it);
        else
            ++it; 
    }
}


glm::vec3 clickToWorldCoord(GLFWwindow* window) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    float ndcX = 2.0f * (mouseX / wWidth) - 1.0f;
    float ndcY = 1.0f - 2.0f * (mouseY / wHeight);

    glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);

    glm::vec4 rayEye = projectionInverse * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::vec3(viewInverse * rayEye);
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

bool checkBombs(glm::vec3 rayWorld) {
    for (const Bomb& bomb : state->bombs) {
        if (isTouching(bomb.pos, rayWorld)) {
            explosionPos = bomb.pos;
            setGameOver();
            return true;
        }
    }
    return false;
}

void checkShot(glm::vec3 rayWorld, bool leftClick) {
    for (auto it = state->balls.begin(); it != state->balls.end(); ++it) {
        if (it->hit)
            continue;
        
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
            return;
        }
    }
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if ((button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_LEFT) && action == GLFW_PRESS) {
        auto ray = clickToWorldCoord(window);
        if (checkBombs(ray)) return;
        checkShot(ray, button == GLFW_MOUSE_BUTTON_LEFT);
    }
}


void setColor(unsigned int shader, char color) {
    switch (color) {
        case 'r': {
            glUniform3f(glGetUniformLocation(shader, "material.ambient"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(shader, "material.diffuse"), 0.45f, 0.f, 0.f);
            glUniform3f(glGetUniformLocation(shader, "material.specular"), 0.7f, 0.6f, 0.6f);
            break;
        }
        case 'b': {
            glUniform3f(glGetUniformLocation(shader, "material.ambient"), 0.0f, 0.1f, 0.06f);
            glUniform3f(glGetUniformLocation(shader, "material.diffuse"), 0.05f, 0.f, 0.45f);
            glUniform3f(glGetUniformLocation(shader, "material.specular"), 0.5f, 0.5f, 0.5f);
            break;
        }
        case 'g': {
            glUniform3f(glGetUniformLocation(shader, "material.ambient"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(shader, "material.diffuse"), 0.22f, 0.25f, 0.25f);
            glUniform3f(glGetUniformLocation(shader, "material.specular"), 0.33f, 0.33f, 0.35f);
            break;
        }
        default: {
            glUniform3f(glGetUniformLocation(shader, "material.ambient"), 0.2f, 0.2f, 0.2f);
            glUniform3f(glGetUniformLocation(shader, "material.diffuse"), 0.65f, 0.65f, 0.65f);
            glUniform3f(glGetUniformLocation(shader, "material.specular"), 0.7f, 0.7f, 0.7f);
        }
    }
}


void generateNewObjects() {
    double t = glfwGetTime();
    if ((state->beatTimes.size() > state->lastBeat) && (state->beatTimes.at(state->lastBeat) - t < 2.6)) {
        generateBall(state->lastBeat);
        state->lastBeat++;
    }

    if ((state->lastBomb < state->bombTimes.size()) && (state->bombTimes.at(state->lastBomb) - t < 2.0)) {
        generateBomb();
        state->lastBomb++;
    }
}

void drawBalls(unsigned int shader, Model& object) {
    glUseProgram(shader);
    for (const auto& ball : state->balls) {
        if (state->mode == 1) {
            if (ball.red) setColor(shader, 'r');
            else setColor(shader, 'b');
        }
        model = glm::mat4(1.0f);
        model = glm::translate(model, ball.pos);
        model = glm::scale(model, glm::vec3(ball.inflation));
        glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(model));
        object.Draw(shader);
    }
    glUseProgram(0);
}

void drawBombs(unsigned int shader, Model& object) {
    glUseProgram(shader);
    setColor(shader, 'g');
    for (const auto& bomb : state->bombs) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, bomb.pos);
        glUniformMatrix4fv(glGetUniformLocation(shader, "uM"), 1, GL_FALSE, glm::value_ptr(model));
        object.Draw(shader);
    }
    glUseProgram(0);
}


void onExplosion() {
    //todo
}

int game(GLFWwindow* window, unsigned int shader, unsigned int rayShader, unsigned int texShader, unsigned int lightShader, GameState* gameState, const char* texturePath) {
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    Model modelBall("resources/model/ball.obj");
    Model modelBomb("resources/model/grenade.obj");
    Model modelTube("resources/model/tube.obj");

    //background logo
    float background[] =
    {   // X       Y    Z     S    T 
        //logo
        -LIMIT, -LIMIT, 0,  0.0, 0.0,
        -LIMIT,  LIMIT, 0,  0.0, 1.0,
         LIMIT, -LIMIT, 0,  1.0, 0.0,
         LIMIT,  LIMIT, 0,  1.0, 1.0,
        //left side
        -LIMIT, -LIMIT, 0.0,     0.0, 0.0,
        -LIMIT,  LIMIT, 0.0,     0.0, 1.0,
        -LIMIT, -LIMIT, Z_LIMIT, 1.0, 0.0,
        -LIMIT,  LIMIT, Z_LIMIT, 1.0, 1.0,
        //right side
         LIMIT, -LIMIT, 0.0,     0.0, 0.0,
         LIMIT,  LIMIT, 0.0,     0.0, 1.0,
         LIMIT, -LIMIT, Z_LIMIT, 1.0, 0.0,
         LIMIT,  LIMIT, Z_LIMIT, 1.0, 1.0,

    };
    unsigned int VAObg, VBObg;
    initVABO(background, sizeof(background), 5 * sizeof(float), &VAObg, &VBObg, true);

    //shaders
    glUseProgram(shader);
    setColor(shader, 'w');
    glUniform1f(glGetUniformLocation(shader, "material.shininess"), 32.0f);
    unsigned int uLightColLoc = glGetUniformLocation(shader, "uLightCol");
    glUniform3f(uLightColLoc, 1.0f, 1.0f, 1.0f);
    
    unsigned int uLightPosLoc = glGetUniformLocation(shader, "uLightPos");
    glUniform3f(uLightPosLoc, lightPosition[0], lightPosition[1], lightPosition[2]);
    
    unsigned int uCamAtLoc = glGetUniformLocation(shader, "uCameraAt");
    glUniform3f(uCamAtLoc, cameraAt[0], cameraAt[1], cameraAt[2]);

    unsigned int modelLoc = glGetUniformLocation(shader, "uM");
    unsigned int projectionViewLoc = glGetUniformLocation(shader, "uPV");

    //aspect-ratio of the window
    glfwGetFramebufferSize(window, &wWidth, &wHeight);
    float aspectRatio = (float)wWidth / wHeight;

    //3D matrices
    model = glm::mat4(1.0f);

    view = glm::lookAt(cameraAt, glm::vec3(0.0f, -3.f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    viewInverse = glm::inverse(view);

    projection = glm::perspective(glm::radians(15.0f), aspectRatio, 0.1f, 100.0f); //Matrica perspektivne projekcije (FOV, Aspect Ratio, prednja ravan, zadnja ravan)
    projectionInverse = glm::inverse(projection);

    projectionView = projection * view;

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //(Adresa matrice, broj matrica koje saljemo, da li treba da se transponuju, pokazivac do matrica)
    glUniformMatrix4fv(projectionViewLoc, 1, GL_FALSE, glm::value_ptr(projectionView));

    glUseProgram(0);

    //shader light
    glUseProgram(lightShader);
    glm::mat4 modelLight = glm::mat4(1.0f);
    unsigned int modelLightLoc = glGetUniformLocation(lightShader, "uM");

    unsigned int projectionViewLightLoc = glGetUniformLocation(lightShader, "uPV");
    glUniformMatrix4fv(projectionViewLightLoc, 1, GL_FALSE, glm::value_ptr(projectionView));
    glUseProgram(0);

    //texture
    unsigned int logoTexture = loadImageToTexture(texturePath);
    unsigned int backgroundTexture = loadImageToTexture("resources/img/stars.jpg");
    glUseProgram(texShader);
    unsigned uTexLoc = glGetUniformLocation(texShader, "uTex");
    glUniform1i(uTexLoc, 0);

    unsigned int modelTexLoc = glGetUniformLocation(texShader, "uM");
    unsigned int projectionViewTexLoc = glGetUniformLocation(texShader, "uPV");
    glUniformMatrix4fv(modelTexLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projectionViewTexLoc, 1, GL_FALSE, glm::value_ptr(projectionView));

    glUseProgram(0);
    

    //options
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    //setting the game state
    state = gameState;
    glfwSetTime(gameState->time);
    setCombo();
    int next = 0;
    bool endGame = false;
    won = false;
    gameOver = false;
    explosionPos = glm::vec3(-1.0);
    std::string scoreTx;
    float scoreScaling = 1.0f;
    std::string comboTx;
    std::string continueTx = "press [SPACE]";

    glClearColor(0., 0., 0.05, 1.0);
    resumeSong(state->song);

    //render loop
    double renderStart, renderTime;
    while (!endGame) {
        renderStart = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            if (won || gameOver) {
                next = 0;   //return to meni
            }
            else {
                pauseSong(state->song);
                gameState->time = glfwGetTime();
                next = 1;   //pause game
            }
            endGame = true;
            break;
        }

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            glEnable(GL_DEPTH_TEST);
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            glDisable(GL_DEPTH_TEST);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //background
        if (logoTexture != 0 && backgroundTexture != 0) {
            glUseProgram(texShader);
            glBindVertexArray(VAObg);
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, logoTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            
            glBindTexture(GL_TEXTURE_2D, backgroundTexture);
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
            glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            glUseProgram(0);
        }

        //draw light tubes
        glUseProgram(lightShader);

        float lightPosX = LIMIT;
        float lightPosY = LIMIT;
        for (int i = 0; i < 2; i++, lightPosX = -lightPosX) {
            for (int j = 0; j < 2; j++, lightPosY = -lightPosY) {
                lightPosition = glm::vec3(lightPosX, lightPosY, 0.0f);
                modelLight = glm::mat4(1.0f);
                modelLight = glm::translate(modelLight, lightPosition);
                modelLight = glm::rotate(modelLight, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                glUniformMatrix4fv(modelLightLoc, 1, GL_FALSE, glm::value_ptr(modelLight));
                modelTube.Draw(lightShader);
            }
        }
        glUseProgram(0);

        if (won || gameOver) {
            renderText(messageTx, 100, wHeight - 200, 2, 1, 1, 1);
            renderText(continueTx, 100, wHeight - 300, 1, 1, 1, 1);
            scoreScaling = 1.5f;
        }

        if (explosionPos[0] > 0) {
            //todo
            onExplosion();
        }

        if (!gameOver) {
            generateNewObjects();
            updateBalls();
            updateBombs();
        }
        drawBalls(shader, modelBall);
        drawBombs(shader, modelBomb);

        scoreTx = "SCORE: " + std::to_string(state->score);
        renderText(scoreTx, 20, 50, scoreScaling, 1.0, 1.0, 1.0);

        comboTx = 'x' + std::to_string(combo);
        renderText(comboTx, 20, 10, 0.8, 1.0, 0.5, 0.);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        //limit FPS
        renderTime = glfwGetTime() - renderStart;
        if (renderTime < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - renderTime));
        }
    }

    glDeleteTextures(1, &logoTexture);
    glDeleteTextures(1, &backgroundTexture);
    glDeleteBuffers(1, &VBObg);
    glDeleteVertexArrays(1, &VAObg);

    return next;
}