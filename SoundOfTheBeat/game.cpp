#include "game.h"
#include <random>


#define CRES 30
#define SPEED 0.05
#define INFLATION_SPEED 0.6
#define r 0.08
#define LIMIT 0.75
#define GEN_LIMIT 0.35
#define Z_LIMIT 8.0
#define DEATH_RAY_Y -LIMIT + 0.05
#define DEATH_RAY_FRAMES 6


std::vector<double> beatTimes;
int lastBeat;
std::vector<Ball> balls;
int score;
int streak;
int combo = 1;
int wWidth, wHeight;
int mode;
int deathRayDuration = 0;   //in frames
float deathRayX;

glm::vec3 cameraAt = glm::vec3(0.0f, 0.2f, Z_LIMIT);
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

void updateBalls() {
    for (auto it = balls.begin(); it != balls.end();) {
        it->pos[2] += SPEED;
        
        if (it->pos[2] >= Z_LIMIT * 0.95 && !it->hit) {   // => miss
            it->hit = true;
            //deathRayX = it->pos[0];
            //deathRayDuration = DEATH_RAY_FRAMES;
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

void checkShot(GLFWwindow* window, bool leftClick) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    float ndcX = 2.0f * (mouseX / wWidth) - 1.0f;
    float ndcY = 1.0f - 2.0f * (mouseY / wHeight);

    glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);

    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
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


int game(GLFWwindow* window, unsigned int shader, unsigned int rayShader, unsigned int texShader, GameState& gameState,  std::vector<double> beats, irrklang::ISound* song, const char* texturePath) {
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    float vertices[(CRES + 3 + 4) * 3]; //+4 for the ray points
    vertices[0] = 0;
    vertices[1] = 0;
    vertices[2] = 0;
    for (int i = 0; i <= CRES; i++)
    {
        vertices[3 + 3 * i] = r * cos((3.141592 / 180) * (i * 360 / CRES));
        vertices[3 + 3 * i + 1] = r * sin((3.141592 / 180) * (i * 360 / CRES));
        vertices[3 + 3 * i + 2] = 0;    //z
    }
    int rayInd = (CRES + 3) * 3;
    int rayEndXAlpha = rayInd + 2;
    int rayEndXBeta = rayInd + 6;
    //concentrated ray (alpha)
    vertices[rayInd] = -1.0;     // x1
    vertices[rayInd + 1] = DEATH_RAY_Y; // y1
    vertices[rayInd + 2] = 1.0;  // z1
    vertices[rayInd + 3] = 1.0;  // x2
    vertices[rayInd + 4] = DEATH_RAY_Y; // y2
    vertices[rayInd + 5] = 1.0; // z2
    //ray effect (beta)
    vertices[rayInd + 6] = -1.0;     // x1
    vertices[rayInd + 7] = DEATH_RAY_Y; // y1
    vertices[rayInd + 8] = 1.0;  // z1
    vertices[rayInd + 9] = 1.0;  // x2
    vertices[rayInd + 10] = DEATH_RAY_Y; // y2
    vertices[rayInd + 11] = 1.0; // z2
    
    unsigned int VAO, VBO;
    initVABO(vertices, sizeof(vertices), 3 * sizeof(float), &VAO, &VBO, true);

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

    //TEST
    /*
    float testver[] =
    {
        //X    Y    Z       R    G    B    A
        0.25, 0.5, 0.75,   1.0, 0.0, 0.0, 0.0, //Crveni trougao - Prednji
       -0.25, 0.5, 0.75,   1.0, 0.0, 0.0, 0.0,
        0.0, -0.5, 0.75,   1.0, 0.0, 0.0, 0.0,

        0.25, -0.5, 0.0,   0.0, 0.0, 1.0, 0.0, //Plavi trougao - Zadnji
       -0.25, -0.5, 0.0,   0.0, 0.0, 1.0, 0.0,
        0.0,   0.5, 0.0,   0.0, 0.0, 1.0, 0.0
    };
    unsigned int stride = (3 + 4) * sizeof(float);
    unsigned int VAOtest, VBOtest;
    glGenVertexArrays(1, &VAOtest);
    glBindVertexArray(VAOtest);

    glGenBuffers(1, &VBOtest);
    glBindBuffer(GL_ARRAY_BUFFER, VBOtest);
    glBufferData(GL_ARRAY_BUFFER, sizeof(testver), testver, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    */
    //TEST END

    //shaders
    glUseProgram(shader);
    unsigned int uColLoc = glGetUniformLocation(shader, "uCol");
    unsigned int inflationLoc = glGetUniformLocation(shader, "uInflation");

    unsigned int modelLoc = glGetUniformLocation(shader, "uM");
    unsigned int projectionViewLoc = glGetUniformLocation(shader, "uPV");

    //aspect-ratio of the window
    glfwGetFramebufferSize(window, &wWidth, &wHeight);
    float aspectRatio = (float)wWidth / wHeight;

    //3D matrices
    glm::mat4 model = glm::mat4(1.0f); //Matrica transformacija - mat4(1.0f) generise jedinicnu matricu

    float yaw = 15;
    float pitch = -90;
    glm::vec3 direction = glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    ) + glm::vec3(0, -1, 0);

    view = glm::lookAt(cameraAt, direction, glm::vec3(0.0f, 1.0f, 0.0f));
     //Matrica pogleda (kamere): lookAt(Gde je kamera, u sta kamera gleda, jedinicni vektor pozitivne Y ose sveta  - ovo rotira kameru)
    viewInverse = glm::inverse(view);

    projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f); //Matrica perspektivne projekcije (FOV, Aspect Ratio, prednja ravan, zadnja ravan)
    projectionInverse = glm::inverse(projection);

    projectionView = projection * view;

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //(Adresa matrice, broj matrica koje saljemo, da li treba da se transponuju, pokazivac do matrica)
    glUniformMatrix4fv(projectionViewLoc, 1, GL_FALSE, glm::value_ptr(projectionView));

    glUseProgram(0);

    unsigned int uAlphaLoc = glGetUniformLocation(rayShader, "uAlpha");

    //texture
    unsigned int logoTexture = loadImageToTexture(texturePath);
    if (logoTexture != 0) {
        glBindTexture(GL_TEXTURE_2D, logoTexture);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(texShader);
        unsigned uTexLoc = glGetUniformLocation(texShader, "uTex");
        glUniform1i(uTexLoc, 0);

        unsigned int modelTexLoc = glGetUniformLocation(texShader, "uM");
        unsigned int projectionViewTexLoc = glGetUniformLocation(texShader, "uPV");
        glUniformMatrix4fv(modelTexLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(projectionViewTexLoc, 1, GL_FALSE, glm::value_ptr(projectionView));

        glUseProgram(0);
    }
    

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
    glfwSetTime(gameState.time);
    lastBeat = gameState.lastBeat;
    int next = 0;

    beatTimes = beats;

    //render loop
    glClearColor(0., 0., 0.1, 1.0);

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
            gameState.time = glfwGetTime();
            gameState.lastBeat = lastBeat;
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

        //background
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
        

        //generate new balls
        double t = glfwGetTime();
        if ((beatTimes.size() > lastBeat) && (beatTimes.at(lastBeat) - t < 2.6)) {
            generateBall(lastBeat);
            lastBeat++;
        }

        //draw balls
        updateBalls();

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glUseProgram(shader);
        glUniform3f(uColLoc, 1.0, 1.0, 1.0);
        for (const auto& ball : balls) {

            if (mode == 1) {
                if (ball.red) glUniform3f(uColLoc, 0.7, 0.05, 0.1);
                else glUniform3f(uColLoc, 0.05, 0., 0.7);
            }

            glUniform1f(inflationLoc, ball.inflation);
            
            model = glm::mat4(1.0f);
            model = glm::translate(model, ball.pos);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLE_FAN, 0, CRES +2);
        }
        glUseProgram(0);

        //TEST
        /*
        glUseProgram(shader);
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(VAOtest);
        glBindBuffer(GL_ARRAY_BUFFER, VBOtest);
        glUniform3f(uColLoc, 0.7, 0.05, 0.1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUniform3f(uColLoc, 0.05, 0., 0.7);
        glDrawArrays(GL_TRIANGLES, 3, 3);
        glUseProgram(0);
        */
        //TEST end

        //death ray definition and rendering
        /*
        if (deathRayDuration > 0) {
            glUseProgram(rayShader);
            deathRayDuration--;
            
            vertices[rayEndXAlpha] = deathRayX;
            vertices[rayEndXBeta] = deathRayX;
            glBufferSubData(GL_ARRAY_BUFFER, rayInd * sizeof(float), 8 * sizeof(float), &vertices[rayInd]);

            glLineWidth(4.0);
            glUniform1f(uAlphaLoc, 1.0);
            glDrawArrays(GL_LINES, rayInd / 2, 2);
            
            glLineWidth(10.0);
            glUniform1f(uAlphaLoc, 0.2);
            glDrawArrays(GL_LINES, rayInd / 2 + 2, 2);
            
            glUseProgram(0);
        }
        */
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

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

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glDeleteTextures(1, &logoTexture);
    glDeleteBuffers(1, &VBOtex);
    glDeleteVertexArrays(1, &VAOtex);

    return next;
}