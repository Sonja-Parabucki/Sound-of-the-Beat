#include "game.h"
#include <random>


#define CRES 30 // circle resoloution
#define SPEED 0.01
#define INFLATION_SPEED 0.6
#define r 0.08
#define LIMIT 0.75
#define DEATH_RAY_Y -LIMIT + 0.05
#define DEATH_RAY_FRAMES 6

const double FPS = 60.0;
const double FRAME_TIME = 1.0 / FPS;

double temp;

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


float randomX() {
    return -LIMIT + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (LIMIT*2)));
}

void setCombo() {
    combo = 1;
    if (streak >= 8) combo = 8;
    else if (streak >= 4) combo = 4;
    else if (streak >= 2) combo = 2;
}

void generateBall(int beatInd) {
    if (beatInd >= beatTimes.size()) {
        std::cout<< beatTimes.size() << "\n";
        return;
    }
    Ball ball{ randomX(), 1.0, beatTimes.at(beatInd), false, 1};
    if (mode == 1)
        ball.red = ball.x <= 0;
    balls.push_back(ball);
}

void updateBalls() {
    for (auto it = balls.begin(); it != balls.end();) {
        it->y -= SPEED;
        
        if (it->y <= DEATH_RAY_Y && !it->hit) {   // => miss
            it->hit = true;
            deathRayX = it->x;
            deathRayDuration = DEATH_RAY_FRAMES;
            score -= 5;
            combo = 1;
            streak = 0;
            //std::cout << score << "\n";
        }

        if (it->hit)
            it->inflation *= INFLATION_SPEED;

        if (it->y < -1.0 - r)   //fell off from the screen
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

    for (auto it = balls.begin(); it != balls.end(); ++it) {
        if (it->hit) {
            continue;
        }
        if (mode == 1 && it->red != leftClick)
            continue;

        if (pow(ndcX - it->x, 2) + pow(ndcY - it->y, 2) <= r*r) {
            double t = glfwGetTime();

            temp = t - it->timeToHit;
            
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

            //std::cout << score << "\n";
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


int game(GLFWwindow* window, unsigned int shader, unsigned int rayShader, unsigned int texShader, GameState& gameState,  std::vector<double> beats, irrklang::ISound* song) {
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    /*
    Generisanje temena kruga po jednacini za kruznicu:
    Trebace nam bar X i Y koordinate, posto je boja u fragment sejderu
    Treba nam 2 * CRES brojeva za X i Y koordinate, gde je CRES zapravo broj temena na kruznici (CRES 6 = sestougao)
    Pored toga nam trebaju jos dva temena - centar i ponovljeno teme ugla 0 (da bi se krug pravilno zatvorio)
    */
    float vertices[(CRES + 2) * 2 + 8];

    vertices[0] = 0;
    vertices[1] = 0;
    for (int i = 0; i <= CRES; i++)
    {
        vertices[2 + 2 * i] = r * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi (Matematicke funkcije rade sa radijanima)
        vertices[2 + 2 * i + 1] = r * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }
    int rayInd = (CRES + 2) * 2;
    int rayEndXAlpha = rayInd + 2;
    int rayEndXBeta = rayInd + 6;
    //concentrated ray 
    vertices[rayInd] = -1.0;     // x1
    vertices[rayInd + 1] = DEATH_RAY_Y; // y1
    vertices[rayInd + 2] = 1.0;  // x2
    vertices[rayInd + 3] = DEATH_RAY_Y;  // y2
    //ray effect
    vertices[rayInd + 4] = -1.0;     // x1
    vertices[rayInd + 5] = DEATH_RAY_Y; // y1
    vertices[rayInd + 6] = 1.0;  // x2
    vertices[rayInd + 7] = DEATH_RAY_Y;  // y2
    
    unsigned int VAO, VBO;
    initVABO(vertices, sizeof(vertices), 2 * sizeof(float), &VAO, &VBO, true);

    //background logo
    float logo[] =
    {   //X        Y      S    T 
        -LIMIT, -LIMIT,  0.0, 0.0,
        -LIMIT,  LIMIT,  0.0, 1.0,
         LIMIT, -LIMIT,  1.0, 0.0,
         LIMIT,  LIMIT,  1.0, 1.0,
    };
    unsigned int stride = 4 * sizeof(float);

    unsigned int VAOtex, VBOtex;
    initVABO(logo, sizeof(logo), 4 * sizeof(float), &VAOtex, &VBOtex, true);



    /*
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    */
    //shaders
    glUseProgram(shader);
    unsigned int uRLoc = glGetUniformLocation(shader, "uR");
    unsigned int uColLoc = glGetUniformLocation(shader, "uCol");
    unsigned int uAspectLoc = glGetUniformLocation(shader, "uAspect");
    unsigned int inflationLoc = glGetUniformLocation(shader, "uInflation");

    //aspect-ratio of the window
    glfwGetFramebufferSize(window, &wWidth, &wHeight);
    float aspectRatio = (float)wHeight / wWidth;
    glUniform1f(uAspectLoc, aspectRatio);
    glUseProgram(0);

    unsigned int uAlphaLoc = glGetUniformLocation(rayShader, "uAlpha");

    //texture
    unsigned int logoTexture = loadImageToTexture("resources/queen.png");
    glBindTexture(GL_TEXTURE_2D, logoTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(texShader);
    unsigned uTexLoc = glGetUniformLocation(texShader, "uTex");
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)
    unsigned int uTexAspectLoc = glGetUniformLocation(texShader, "uAspect");
    glUniform1f(uTexAspectLoc, aspectRatio);
    glUseProgram(0);

    //options
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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

        if (balls.empty() && lastBeat == beats.size()) {
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
        glClear(GL_COLOR_BUFFER_BIT);

        //background
        glUseProgram(texShader);
        glBindVertexArray(VAOtex);
        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, logoTexture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);


        //generate new balls
        double t = glfwGetTime();
        if ((beats.size() > lastBeat) && (beats.at(lastBeat) - t < 0.5)) {
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
            glUniform2f(uRLoc, ball.x, ball.y);
            glDrawArrays(GL_TRIANGLE_FAN, 0, CRES +2);
        }
        glUseProgram(0);

        //death ray definition and rendering
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
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        scoreTx = "SCORE: " + std::to_string(score);
        renderText(scoreTx, 20, 50, 1);

        comboTx = 'x' + std::to_string(combo);
        renderText(comboTx, 20, 10, 0.8);
        renderText("TEMP:" + std::to_string(temp), 0, 0, 0.5);
        
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