#include "game.h"
#include <random>


#define CRES 30 // Rezolucija kruga
#define SPEED 0.001
#define r 0.08

struct Ball {
    float x, y;
    //vreme kad da se pojavi i kad treba da se stisne
    //svaka svoju brzinu?
};

std::vector<Ball> balls;

float randomX() {
    float limit = 1 - r;
    return -limit + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (limit*2)));
}

void generateBall() {
    Ball ball{randomX(), 1.0};
    balls.push_back(ball);
}

void updateBalls() {
    for (auto it = balls.begin(); it != balls.end();) {
        it->y -= SPEED;
        if (it->y < -1.0 - r)
            it = balls.erase(it);
        else
            ++it;
    }
}



int game(GLFWwindow* window, unsigned int shader) {
    
    /*
    Generisanje temena kruga po jednacini za kruznicu:
    Trebace nam bar X i Y koordinate, posto je boja u fragment sejderu
    Treba nam 2 * CRES brojeva za X i Y koordinate, gde je CRES zapravo broj temena na kruznici (CRES 6 = sestougao)
    Pored toga nam trebaju jos dva temena - centar i ponovljeno teme ugla 0 (da bi se krug pravilno zatvorio)
    */
    float circle[(CRES + 2) * 2];

    circle[0] = 0; //Centar X0
    circle[1] = 0; //Centar Y0
    for (int i = 0; i <= CRES; i++)
    {
        circle[2 + 2 * i] = r * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi (Matematicke funkcije rade sa radijanima)
        circle[2 + 2 * i + 1] = r * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);//(indeks pokazivaca, broj komponenti atributa, tip komponenti atributa, da li je potrebno normalizovati podatke (nama uvek GL_FALSE), korak/velicina temena, pomeraj sa pocetka jednog temena do komponente za ovaj atribut - mora biti (void*))  
    glEnableVertexAttribArray(0);

    glUseProgram(shader);
    unsigned int uRLoc = glGetUniformLocation(shader, "uR");
    unsigned int uColLoc = glGetUniformLocation(shader, "uCol");
    unsigned int uAspectLoc = glGetUniformLocation(shader, "uAspect");
    glUniform2f(uRLoc, 1.0, 1.0);
    glUniform3f(uColLoc, 0.0, 0.0, 0.0);

    //aspect-ratio of the window
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspectRatio = (float)height / width;
    glUniform1f(uAspectLoc, aspectRatio);

    int score = 10;
    //render petlja
    glClearColor(0.1, 0.1, 0.1, 1.0);

    int i = 0;
    glfwSetTime(0);

    int spawnTimer = 0;
    
    while (score >= 0) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            //todo: pauziraj
        }
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        {
            //trenutno povratak na meni
            score  = -1;
        }
        glClear(GL_COLOR_BUFFER_BIT);

        int t = glfwGetTime();
        //std::cout << t << ' ';

        if (t - spawnTimer > 2) {
            generateBall();
            spawnTimer = t;
        }
        updateBalls();

        glUniform3f(uColLoc, 1.0, 1.0, 1.0);
        for (const auto& ball : balls) {
            glUniform2f(uRLoc, ball.x, ball.y);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));
        }
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    return score;
}