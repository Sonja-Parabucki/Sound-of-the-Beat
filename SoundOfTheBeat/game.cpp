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


std::vector<double> beatTimes;
int lastBeat;
std::vector<double> bombTimes;
int lastBomb;
std::vector<Ball> balls;
std::vector<Bomb> bombs;
int score;
int streak;
int combo = 1;
int wWidth, wHeight;
int mode;

glm::vec3 lightPosition = glm::vec3(LIMIT, LIMIT, 0.0f);

glm::vec3 cameraAt = glm::vec3(0.0f, 0.5f, Z_LIMIT);
glm::mat4 projectionView;
glm::mat4 view;
glm::mat4 viewInverse;
glm::mat4 projection;
glm::mat4 projectionInverse;


float random() {
    return -GEN_LIMIT + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (GEN_LIMIT *2)));
}

float random(float a, float b) {
    return a + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (b-a)));
}

void setCombo() {
    combo = 1;
    if (streak >= 8) combo = 8;
    else if (streak >= 4) combo = 4;
    else if (streak >= 2) combo = 2;
}

void generateBall(int beatInd) {
    if (beatInd >= beatTimes.size()) {
        return;
    }
    Ball ball{ glm::vec3(random(), random(), 0.f), beatTimes.at(beatInd), false, 1};

    if (mode == 1)
        ball.red = ball.pos[0] <= 0;
    balls.push_back(ball);
}

void generateBomb() {
    Bomb bomb{ glm::vec3(random(), random(), 0.f) };
    bombs.push_back(bomb);
}

void updateBalls() {
    for (auto it = balls.begin(); it != balls.end();) {
        it->pos[2] += SPEED;
        
        if (it->pos[2] >= Z_LIMIT * 0.95 && !it->hit) {   // => miss
            it->hit = true;
            score -= 5;
            combo = 1;
            streak = 0;
            playRay();
        }

        if (it->hit)
            it->inflation *= INFLATION_SPEED;

        if (it->pos[2] > Z_LIMIT)   //fell off from the screen
            it = balls.erase(it);
        else
            ++it;
    }
}

void updateBombs() {
    for (auto it = bombs.begin(); it != bombs.end();) {
        it->pos[2] += BOMB_SPEED;
        if (it->pos[2] > Z_LIMIT)   //fell off from the screen
            it = bombs.erase(it);
        else
            ++it; 
    }
}

void checkShot(GLFWwindow* window, bool leftClick) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    float ndcX = 2.0f * (mouseX / wWidth) - 1.0f;
    float ndcY = 1.0f - 2.0f * (mouseY / wHeight);

    glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);

    glm::vec4 rayEye = projectionInverse * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::vec3(viewInverse * rayEye);
    rayWorld = glm::normalize(rayWorld);

    for (auto it = balls.begin(); it != balls.end(); ++it) {
        if (it->hit)
            continue;
        
        if (mode == 1 && it->red != leftClick)
            continue;

        glm::vec3 oc = cameraAt - it->pos;
        float a = glm::dot(rayWorld, rayWorld);
        float b = 2.0f * glm::dot(oc, rayWorld);
        float c = glm::dot(oc, oc) - r * r;
        float discriminant = b * b - 4 * a * c;

        if (discriminant >= 0) {
            double t = glfwGetTime();
            if (abs(t - it->timeToHit) < 0.5) {
                streak += 1;
                setCombo();
                score += 2 * combo;
                playEffect();
            }
            else {
                //the hit is early or late => lose the streak, no points gained or lost
                streak = 0;
                setCombo();
                playMiss();
            }

            it->hit = true;
            return;
        }
    }
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if ((button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_LEFT) && action == GLFW_PRESS)
            checkShot(window, button == GLFW_MOUSE_BUTTON_LEFT);
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


int game(GLFWwindow* window, unsigned int shader, unsigned int rayShader, unsigned int texShader, unsigned int lightShader, GameState& gameState, irrklang::ISound* song, const char* texturePath) {
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    Model modelBall("resources/model/ball.obj");
    Model modelBomb("resources/model/grenade.obj");

    //background logo
    float logo[] =
    {   // X       Y    Z     S    T 
        -LIMIT, -LIMIT, 0,  0.0, 0.0,
        -LIMIT,  LIMIT, 0,  0.0, 1.0,
         LIMIT, -LIMIT, 0,  1.0, 0.0,
         LIMIT,  LIMIT, 0,  1.0, 1.0,
    };
    unsigned int VAOtex, VBOtex;
    initVABO(logo, sizeof(logo), 5 * sizeof(float), &VAOtex, &VBOtex, true);

    //background logo
    float background[] =
    {   //left side
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

    Model modelTube("resources/model/tube.obj");

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
    glm::mat4 model = glm::mat4(1.0f);

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
    mode = gameState.mode;
    score = gameState.score;
    streak = gameState.streak;
    setCombo();

    balls = gameState.balls;
    bombs = gameState.bombs;
    glfwSetTime(gameState.time);

    beatTimes = gameState.beatTimes;
    lastBeat = gameState.lastBeat;

    bombTimes = gameState.bombTimes;
    lastBomb = gameState.lastBomb;
    int next = 0;


    //render loop
    glClearColor(0., 0., 0.05, 1.0);

    int i = 0;
    bool endGame = false;
    std::string scoreTx;
    std::string comboTx;

    resumeSong(song);

    double renderStart, renderTime;
    while (score > 0 && !endGame) {
        renderStart = glfwGetTime();

        if (balls.empty() && lastBeat == beatTimes.size()) {
            gameState.score = score;
            gameState.streak = streak;
            gameState.balls = balls;
            gameState.time = glfwGetTime();
            next = 0;
            endGame = true;
            break;
        }

        //pause game
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            pauseSong(song);
            gameState.score = score;
            gameState.streak = streak;
            gameState.balls = balls;
            gameState.bombs = bombs;
            gameState.time = glfwGetTime();
            gameState.lastBeat = lastBeat;
            gameState.lastBomb = lastBomb;
            next = 1;
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

        //logo
        if (logoTexture != 0) {
            glUseProgram(texShader);
            glBindVertexArray(VAOtex);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, logoTexture);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            glUseProgram(0);
        }
        
        //background
        if (backgroundTexture != 0) {
            glUseProgram(texShader);
            glBindVertexArray(VAObg);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, backgroundTexture);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

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

        //generate new balls
        double t = glfwGetTime();
        if ((beatTimes.size() > lastBeat) && (beatTimes.at(lastBeat) - t < 2.6)) {
            generateBall(lastBeat);
            lastBeat++;
        }

        //draw balls
        updateBalls();

        glUseProgram(shader);
        for (const auto& ball : balls) {

            if (mode == 1) {
                if (ball.red) setColor(shader, 'r');
                else setColor(shader, 'b');
            }
            
            model = glm::mat4(1.0f);
            model = glm::translate(model, ball.pos);
            model = glm::scale(model, glm::vec3(ball.inflation));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            modelBall.Draw(shader);
        }
        glUseProgram(0);

        //generate bomb
        if ((lastBomb < bombTimes.size()) && (bombTimes.at(lastBomb) - t < 2.0)) {
            generateBomb();
            lastBomb++;
        }

        //draw bombs
        updateBombs();

        glUseProgram(shader);
        setColor(shader, 'g');
        for (const auto& bomb : bombs) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, bomb.pos);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            modelBomb.Draw(shader);
        }
        glUseProgram(0);

        scoreTx = "SCORE: " + std::to_string(score);
        renderText(scoreTx, 20, 50, 1, 1.0, 1.0, 1.0);

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
    glDeleteBuffers(1, &VBOtex);
    glDeleteVertexArrays(1, &VAOtex);

    glDeleteTextures(1, &backgroundTexture);
    glDeleteBuffers(1, &VBObg);
    glDeleteVertexArrays(1, &VAObg);

    return next;
}