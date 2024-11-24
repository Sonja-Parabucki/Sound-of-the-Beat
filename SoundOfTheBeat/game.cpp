#include "game.h"
#include <random>


#define CRES 30 // Rezolucija kruga
#define SPEED 0.01
#define INFLATION_SPEED 0.6
#define r 0.08
#define DEATH_RAY_Y -0.8
#define DEATH_RAY_FRAMES 10

const double FPS = 60.0;
const double FRAME_TIME = 1.0 / FPS;


std::vector<Ball> balls;
int score;
int wWidth, wHeight;
int mode;
int deathRayDuration = 0;   //in frames
float deathRayX;


float randomX() {
    float limit = 1 - r;
    return -limit + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (limit*2)));
}

void generateBall() {
    Ball ball{ randomX(), 1.0, false, 1 };
    if (mode == 1)
        ball.red = (int)glfwGetTime() % 2 == 0;
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
            std::cout << score << "\n";
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
            score += 10;
            //todo: update score display
            std::cout << score << "\n";
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




int game(GLFWwindow* window, unsigned int shader, unsigned int rayShader, GameState& gameState) {
    
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

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);//(indeks pokazivaca, broj komponenti atributa, tip komponenti atributa, da li je potrebno normalizovati podatke (nama uvek GL_FALSE), korak/velicina temena, pomeraj sa pocetka jednog temena do komponente za ovaj atribut - mora biti (void*))  
    glEnableVertexAttribArray(0);

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


    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);


    mode = gameState.mode;
    score = gameState.score;
    balls = gameState.balls;
    glfwSetTime(gameState.time);
    gameState.next = 0;

    //render petlja
    glClearColor(0.1, 0.1, 0.1, 1.0);

    int i = 0;
    bool endGame = false;

    int spawnTimer = 0;
    double renderStart, renderTime;
    while (score > 0 && !endGame) {
        renderStart = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            gameState.score = score;
            gameState.balls = balls;
            gameState.time = glfwGetTime();
            gameState.next = 1;
            endGame = true;
            break;
        }
        glClear(GL_COLOR_BUFFER_BIT);

        int t = glfwGetTime();
        if (t - spawnTimer >= 1) {
            generateBall();
            spawnTimer = t;
        }
        updateBalls();

        glUseProgram(shader);
        glUniform3f(uColLoc, 1.0, 1.0, 1.0);
        for (const auto& ball : balls) {

            if (mode == 1) {
                if (ball.red) glUniform3f(uColLoc, 1.0, 0.0, 0.0);
                else glUniform3f(uColLoc, 0.0, 0.0, 1.0);
            }

            glUniform1f(inflationLoc, ball.inflation);
            glUniform2f(uRLoc, ball.x, ball.y);
            glDrawArrays(GL_TRIANGLE_FAN, 0, CRES +2);
        }
        glUseProgram(0);

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
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        //limit FPS
        renderTime = glfwGetTime() - renderStart;
        std::cout << renderTime << std::endl;
        if (renderTime < FRAME_TIME) {
            std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - renderTime));
        }
    }

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    return gameState.next;
}