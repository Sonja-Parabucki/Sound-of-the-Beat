
#include "menu.h"
#include "game.h"

#define CRES 30 // Circle Resolution = Rezolucija kruga


int main()
{
    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    const char windowTitle[] = "Sound of the Beat";
    GLFWwindow* window = glfwCreateWindow(windowWidth(), windowHeight(), windowTitle, glfwGetPrimaryMonitor(), NULL);
    if (window == NULL) {
        std::cout << "Failed to create window";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to load GLEW";
        return 3;
    }

    unsigned int basicShader = createShader("basic.vert", "basic.frag"); // Napravi objedinjeni sejder program
    unsigned int ballShader = createShader("ball.vert", "ball.frag");
    unsigned int rayShader = createShader("basic.vert", "ray.frag");


    //todo: ucita najbolji rezultat i poslednji rezultat iz fajla
    int highScore = 0;
    int score;
    int mode = 0;
    while (true) {
        if (menu(window, basicShader) == 0) {
            glDeleteProgram(basicShader);
            glDeleteProgram(ballShader);

            glfwTerminate();
            return 0;
        }
        score = game(window, ballShader, rayShader, mode);

        if (score > highScore) {
            highScore = score;
            //todo: upise u fajl
        }
    }

}


